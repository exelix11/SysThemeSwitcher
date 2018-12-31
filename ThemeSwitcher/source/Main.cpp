
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <malloc.h>
#include <switch.h>
#include "fs.hpp"
#include <filesystem>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

extern "C" {
    extern u32 __start__;

    u32 __nx_applet_type = AppletType_None;

    size_t nx_inner_heap_size = KIP_HEAP;
    char   nx_inner_heap[KIP_HEAP];
    
    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);
}

void __libnx_initheap(void) {
    void*  addr = nx_inner_heap;
    size_t size = nx_inner_heap_size;

    /* Newlib */
    extern char* fake_heap_start;
    extern char* fake_heap_end;

    fake_heap_start = (char*)addr;
    fake_heap_end   = (char*)addr + size;
}
struct fatalLaterIpc
{
    u64 magic;
    u64 cmd_id;
    u64 result;
    u64 unknown;
};
void fatalLater(Result err)
{
    Handle srv;

    while (R_FAILED(smGetServiceOriginal(&srv, smEncodeName("fatal:u"))))
    {
        // wait one sec and retry
        svcSleepThread(1000000000L);
    }

    // fatal is here time, fatal like a boss
    IpcCommand c;
    ipcInitialize(&c);
    ipcSendPid(&c);

    struct fatalLaterIpc* raw;

    raw = (struct fatalLaterIpc*) ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 1;
    raw->result = err;
    raw->unknown = 0;

    ipcDispatch(srv);
    svcCloseHandle(srv);
}

void registerFspLr()
{
    if (kernelAbove400())
        return;

    Result rc = fsprInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);

    u64 pid;
    svcGetProcessId(&pid, CUR_PROCESS_HANDLE);

    rc = fsprRegisterProgram(pid, KIP_TITLEID, FsStorageId_NandSystem, NULL, 0, NULL, 0);
    if (R_FAILED(rc))
        fatalLater(rc);
    fsprExit();
}

void __appInit(void) {
    Result rc;
    svcSleepThread(10000000000L);
    
    rc = smInitialize();
    if (R_FAILED(rc)) {
        fatalLater(rc);
    }
    
    rc = fsInitialize();
    if (R_FAILED(rc)) {
        fatalLater(rc);
    }
    registerFspLr();
    
    rc = fsdevMountSdmc();
    if (R_FAILED(rc)) {
        fatalLater(rc);
    }
}

void __appExit(void) {
    /* Cleanup services. */
    fsdevUnmountAll();
    fsExit();
    smExit();
}


using namespace std;

int main()
{
	CheckThemesFolder();
	{
		auto res = SearchCfwFolders();
		if (res.size() != 1)
			return 0;
		CfwFolder = res[0];
	}
	
	auto themes = GetShuffleTargets();	
	if (themes.size() < 2)
		return 0;
	
	std::sort(themes.begin(), themes.end());
	
	int index = 0;
	FILE *indexFile = fopen("sdmc:/themes/shuffle/index.db", "r");
	if (indexFile)
	{
		fscanf(indexFile,"%d",&index);
		fclose(indexFile);
	}
	
	if (index >= 0)
	{
		if (index + 1 >= themes.size())
			index = 0;
		else
			index++;
	}
	else 
	{
		srand(time(0));
		index = rand() % themes.size();
	}
	
    svcSleepThread(10000000000L);
	InstallShuffleTarget(themes[index]);	
	
	indexFile = fopen("sdmc:/themes/shuffle/index.db", "w");
	fprintf(indexFile,"%d",index);
	fflush(indexFile);
	fclose(indexFile);
	
	return 0;
}