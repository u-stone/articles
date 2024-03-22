[toc]

## `quartz!CFilterGraph::ConnectDirect`

### 实现

<details>
<summary>汇编代码</summary>

```asm
0:003> uf quartz!CFilterGraph::ConnectDirect
quartz!CFilterGraph::ConnectDirect:
00007ffc`2e1510c0 48895c2410      mov     qword ptr [rsp+10h],rbx
00007ffc`2e1510c5 48896c2418      mov     qword ptr [rsp+18h],rbp
00007ffc`2e1510ca 56              push    rsi
00007ffc`2e1510cb 57              push    rdi
00007ffc`2e1510cc 4156            push    r14
00007ffc`2e1510ce 4883ec20        sub     rsp,20h
00007ffc`2e1510d2 83a11802000000  and     dword ptr [rcx+218h],0
00007ffc`2e1510d9 4d8bf1          mov     r14,r9
00007ffc`2e1510dc 498bf0          mov     rsi,r8
00007ffc`2e1510df 488bea          mov     rbp,rdx
00007ffc`2e1510e2 488bf9          mov     rdi,rcx
00007ffc`2e1510e5 4885d2          test    rdx,rdx
00007ffc`2e1510e8 0f84b0000000    je      quartz!CFilterGraph::ConnectDirect+0xde (00007ffc`2e15119e)  Branch

quartz!CFilterGraph::ConnectDirect+0x2e:
00007ffc`2e1510ee 4d85c0          test    r8,r8
00007ffc`2e1510f1 0f84a7000000    je      quartz!CFilterGraph::ConnectDirect+0xde (00007ffc`2e15119e)  Branch

quartz!CFilterGraph::ConnectDirect+0x37:
00007ffc`2e1510f7 e8d0f40000      call    quartz!CFilterGraph::CheckPinInGraph (00007ffc`2e1605cc)
00007ffc`2e1510fc 85c0            test    eax,eax
00007ffc`2e1510fe 0f889f000000    js      quartz!CFilterGraph::ConnectDirect+0xe3 (00007ffc`2e1511a3)  Branch

quartz!CFilterGraph::ConnectDirect+0x44:
00007ffc`2e151104 488bd6          mov     rdx,rsi
00007ffc`2e151107 488bcf          mov     rcx,rdi
00007ffc`2e15110a e8bdf40000      call    quartz!CFilterGraph::CheckPinInGraph (00007ffc`2e1605cc)
00007ffc`2e15110f 85c0            test    eax,eax
00007ffc`2e151111 0f888c000000    js      quartz!CFilterGraph::ConnectDirect+0xe3 (00007ffc`2e1511a3)  Branch

quartz!CFilterGraph::ConnectDirect+0x57:
00007ffc`2e151117 488d9700010000  lea     rdx,[rdi+100h]
00007ffc`2e15111e 4183c8ff        or      r8d,0FFFFFFFFh
00007ffc`2e151122 488d4c2440      lea     rcx,[rsp+40h]
00007ffc`2e151127 e8b0ddffff      call    quartz!CAutoMsgMutex::CAutoMsgMutex (00007ffc`2e14eedc)
00007ffc`2e15112c 488d9f80010000  lea     rbx,[rdi+180h]
00007ffc`2e151133 4d8bce          mov     r9,r14
00007ffc`2e151136 ff03            inc     dword ptr [rbx]
00007ffc`2e151138 4c8bc6          mov     r8,rsi
00007ffc`2e15113b 488bd5          mov     rdx,rbp
00007ffc`2e15113e 488bcf          mov     rcx,rdi
00007ffc`2e151141 e87a000000      call    quartz!CFilterGraph::ConnectDirectInternal (00007ffc`2e1511c0)
00007ffc`2e151146 ff8794010000    inc     dword ptr [rdi+194h]
00007ffc`2e15114c 488bcf          mov     rcx,rdi
00007ffc`2e15114f 8bf0            mov     esi,eax
00007ffc`2e151151 e806fbffff      call    quartz!CFilterGraph::AttemptDeferredConnections (00007ffc`2e150c5c)
00007ffc`2e151156 488bcb          mov     rcx,rbx
00007ffc`2e151159 e8c2c40000      call    quartz!CReconnectList::Passive (00007ffc`2e15d620)
00007ffc`2e15115e 488b4c2440      mov     rcx,qword ptr [rsp+40h]
00007ffc`2e151163 4885c9          test    rcx,rcx
00007ffc`2e151166 741c            je      quartz!CFilterGraph::ConnectDirect+0xc4 (00007ffc`2e151184)  Branch

quartz!CFilterGraph::ConnectDirect+0xa8:
00007ffc`2e151168 83410cff        add     dword ptr [rcx+0Ch],0FFFFFFFFh
00007ffc`2e15116c 7516            jne     quartz!CFilterGraph::ConnectDirect+0xc4 (00007ffc`2e151184)  Branch

quartz!CFilterGraph::ConnectDirect+0xae:
00007ffc`2e15116e c7410800000000  mov     dword ptr [rcx+8],0
00007ffc`2e151175 488b09          mov     rcx,qword ptr [rcx]
00007ffc`2e151178 48ff1509e71000  call    qword ptr [quartz!_imp_ReleaseMutex (00007ffc`2e25f888)]
00007ffc`2e15117f 0f1f440000      nop     dword ptr [rax+rax]

quartz!CFilterGraph::ConnectDirect+0xc4:
00007ffc`2e151184 488bcf          mov     rcx,rdi
00007ffc`2e151187 e894340000      call    quartz!CFilterGraph::NotifyChange (00007ffc`2e154620)
00007ffc`2e15118c 85f6            test    esi,esi
00007ffc`2e15118e 780a            js      quartz!CFilterGraph::ConnectDirect+0xda (00007ffc`2e15119a)  Branch

quartz!CFilterGraph::ConnectDirect+0xd0:
00007ffc`2e151190 c7879801000001000000 mov dword ptr [rdi+198h],1

quartz!CFilterGraph::ConnectDirect+0xda:
00007ffc`2e15119a 8bc6            mov     eax,esi
00007ffc`2e15119c eb05            jmp     quartz!CFilterGraph::ConnectDirect+0xe3 (00007ffc`2e1511a3)  Branch

quartz!CFilterGraph::ConnectDirect+0xde:
00007ffc`2e15119e b803400080      mov     eax,80004003h

quartz!CFilterGraph::ConnectDirect+0xe3:
00007ffc`2e1511a3 488b5c2448      mov     rbx,qword ptr [rsp+48h]
00007ffc`2e1511a8 488b6c2450      mov     rbp,qword ptr [rsp+50h]
00007ffc`2e1511ad 4883c420        add     rsp,20h
00007ffc`2e1511b1 415e            pop     r14
00007ffc`2e1511b3 5f              pop     rdi
00007ffc`2e1511b4 5e              pop     rsi
00007ffc`2e1511b5 c3              ret
```
</details>

### wt 跟踪

<details>

<summary>WinDbg wt 命令执行结果</summary>

```
Opened log file 'd:/tmp/connect_62f4_2023-11-09_17-46-55-035.log'
0:020> bl
bl
     0 e Disable Clear  00007ffc`2e1510c0     0001 (0001)  0:**** quartz!CFilterGraph::ConnectDirect
0:020> g
g
Breakpoint 0 hit
quartz!CFilterGraph::ConnectDirect:
00007ffc`2e1510c0 48895c2410      mov     qword ptr [rsp+10h],rbx ss:000000cb`60cfe9c8={nertc_sdk_d!MEDIASUBTYPE_dvhd (00007ffc`2704ff10)}
0:025> wt -m nertc_sdk_d -m quartz -m ntdll -m KERNEL32 
wt -m nertc_sdk_d -m quartz -m ntdll -m KERNEL32 
Tracing quartz!CFilterGraph::ConnectDirect to return address 00007ffc`248c63d6
Breakpoint 0 hit
quartz!CFilterGraph::ConnectDirect:
00007ffc`2e1510c0 48895c2410      mov     qword ptr [rsp+10h],rbx ss:000000cb`60cfe9c8={nertc_sdk_d!MEDIASUBTYPE_dvhd (00007ffc`2704ff10)}
0:025> wt -m nertc_sdk_d -m quartz -m ntdll -m KERNEL32 
wt -m nertc_sdk_d -m quartz -m ntdll -m KERNEL32 
Tracing quartz!CFilterGraph::ConnectDirect to return address 00007ffc`248c63d6
   16     0 [  0] quartz!CFilterGraph::ConnectDirect
   15     0 [  1]   quartz!CFilterGraph::CheckPinInGraph
    1     0 [  2]     quartz!guard_dispatch_icall_nop
   37     0 [  2]     ksproxy!CBasePin::QueryPinInfo
   21    38 [  1]   quartz!CFilterGraph::CheckPinInGraph
   14     0 [  2]     quartz!CFilterGraph::CheckFilterInGraph
    1     0 [  3]       quartz!guard_dispatch_icall_nop
   30     0 [  3]       ksproxy!CBaseFilter::QueryFilterInfo
   21    31 [  2]     quartz!CFilterGraph::CheckFilterInGraph
   12     0 [  3]       quartz!IsEqualObject
   29    43 [  2]     quartz!CFilterGraph::CheckFilterInGraph
    1     0 [  3]       quartz!guard_dispatch_icall_nop
    4     0 [  3]       quartz!CFilterGraph::Release
    1     0 [  3]       quartz!guard_dispatch_icall_nop
    9     0 [  3]       quartz!CUnknown::NonDelegatingRelease
   34    58 [  2]     quartz!CFilterGraph::CheckFilterInGraph
    6     0 [  3]       quartz!_security_check_cookie
   37    64 [  2]     quartz!CFilterGraph::CheckFilterInGraph
   26   139 [  1]   quartz!CFilterGraph::CheckPinInGraph
    1     0 [  2]     quartz!guard_dispatch_icall_nop
    4     0 [  2]     ksproxy!CBaseAllocator::Release
    1     0 [  2]     ksproxy!guard_dispatch_icall_nop
   11     0 [  2]     ksproxy!CUnknown::NonDelegatingRelease
   31   156 [  1]   quartz!CFilterGraph::CheckPinInGraph
    6     0 [  2]     quartz!_security_check_cookie
   35   162 [  1]   quartz!CFilterGraph::CheckPinInGraph
   21   197 [  0] quartz!CFilterGraph::ConnectDirect
   15     0 [  1]   quartz!CFilterGraph::CheckPinInGraph
    1     0 [  2]     quartz!guard_dispatch_icall_nop
    1     0 [  2]     nertc_sdk_d!ILT+887535(?QueryPinInfoCaptureInputPinvideocapturemodulewebrtcEEAAJPEAU_PinInfoZ)
  290     0 [  2]     nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryPinInfo
    1     0 [  3]       nertc_sdk_d!ILT+146655(?AddRef?$ComRefCountVCaptureSinkFiltervideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  3]       nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::AddRef
    1     0 [  4]         nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc_implwebrtcQEAAXXZ)
   18     0 [  4]         nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    1     0 [  5]           nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSAHPECHZ)
    9     0 [  5]           nertc_sdk_d!rtc::AtomicOps::Increment
   21    10 [  4]         nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    9    32 [  3]       nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::AddRef
  295    42 [  2]     nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryPinInfo
   21   339 [  1]   quartz!CFilterGraph::CheckPinInGraph
   14     0 [  2]     quartz!CFilterGraph::CheckFilterInGraph
    1     0 [  3]       quartz!guard_dispatch_icall_nop
    1     0 [  3]       nertc_sdk_d!ILT+418995(?QueryFilterInfoCaptureSinkFiltervideocapturemodulewebrtcUEAAJPEAU_FilterInfoZ)
  282     0 [  3]       nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::QueryFilterInfo
    4     0 [  4]         quartz!CFilterGraph::AddRef
    1     0 [  4]         quartz!guard_dispatch_icall_nop
    6     0 [  4]         quartz!CUnknown::NonDelegatingAddRef
  287    11 [  3]       nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::QueryFilterInfo
   21   300 [  2]     quartz!CFilterGraph::CheckFilterInGraph
   12     0 [  3]       quartz!IsEqualObject
   29   312 [  2]     quartz!CFilterGraph::CheckFilterInGraph
    1     0 [  3]       quartz!guard_dispatch_icall_nop
    4     0 [  3]       quartz!CFilterGraph::Release
    1     0 [  3]       quartz!guard_dispatch_icall_nop
    9     0 [  3]       quartz!CUnknown::NonDelegatingRelease
   34   327 [  2]     quartz!CFilterGraph::CheckFilterInGraph
    6     0 [  3]       quartz!_security_check_cookie
   37   333 [  2]     quartz!CFilterGraph::CheckFilterInGraph
   26   709 [  1]   quartz!CFilterGraph::CheckPinInGraph
    1     0 [  2]     quartz!guard_dispatch_icall_nop
    1     0 [  2]     nertc_sdk_d!ILT+609390(?Release?$ComRefCountVCaptureSinkFiltervideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  2]     nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::Release
    1     0 [  3]       nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_implwebrtcQEAA?AW4RefCountReleaseStatusrtcXZ)
   22     0 [  3]       nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
    1     0 [  4]         nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcSAHPECHZ)
    9     0 [  4]         nertc_sdk_d!rtc::AtomicOps::Decrement
   29    10 [  3]       nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
   12    40 [  2]     nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::Release
   31   763 [  1]   quartz!CFilterGraph::CheckPinInGraph
    6     0 [  2]     quartz!_security_check_cookie
   35   769 [  1]   quartz!CFilterGraph::CheckPinInGraph
   27  1001 [  0] quartz!CFilterGraph::ConnectDirect
    9     0 [  1]   quartz!CAutoMsgMutex::CAutoMsgMutex
   11     0 [  2]     quartz!CMsgMutex::Lock
    3     0 [  3]       KERNEL32!GetCurrentThreadId
   25     3 [  2]     quartz!CMsgMutex::Lock
   32     0 [  3]       quartz!WaitDispatchingMessages
    1     0 [  4]         KERNEL32!WaitForMultipleObjectsEx
   85     0 [  4]         KERNELBASE!WaitForMultipleObjectsEx
   50    86 [  3]       quartz!WaitDispatchingMessages
   38   139 [  2]     quartz!CMsgMutex::Lock
   18   177 [  1]   quartz!CAutoMsgMutex::CAutoMsgMutex
   34  1196 [  0] quartz!CFilterGraph::ConnectDirect
   25     0 [  1]   quartz!CFilterGraph::ConnectDirectInternal
   16     0 [  2]     quartz!CAutoTimer::CAutoTimer
    9     0 [  3]       quartz!CStats::Find
   10     0 [  4]         ntdll!RtlEnterCriticalSection
   19    10 [  3]       quartz!CStats::Find
    1     0 [  4]         KERNEL32!lstrcmpiWStub
  244     0 [  4]         KERNELBASE!lstrcmpiW
   24   255 [  3]       quartz!CStats::Find
   19     0 [  4]         ntdll!RtlLeaveCriticalSection
   32   274 [  3]       quartz!CStats::Find
   23   306 [  2]     quartz!CAutoTimer::CAutoTimer
    1     0 [  3]       KERNEL32!QueryPerformanceCounterStub
   37     0 [  3]       ntdll!RtlQueryPerformanceCounter
   37   344 [  2]     quartz!CAutoTimer::CAutoTimer
   29   381 [  1]   quartz!CFilterGraph::ConnectDirectInternal
   23     0 [  2]     quartz!CFilterGraph::IsUpstreamOf
   16     0 [  3]       quartz!CFilterGraph::FindOutputPinsHelper
   22     0 [  4]         quartz!CFilterGraph::FindOutputPins2
    1     0 [  5]           quartz!guard_dispatch_icall_nop
    1     0 [  5]           nertc_sdk_d!ILT+550225(?QueryInternalConnectionsCaptureInputPinvideocapturemodulewebrtcEEAAJPEAPEAUIPinPEAKZ)
    5     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryInternalConnections
   32     7 [  4]         quartz!CFilterGraph::FindOutputPins2
    1     0 [  5]           quartz!guard_dispatch_icall_nop
    1     0 [  5]           nertc_sdk_d!ILT+887535(?QueryPinInfoCaptureInputPinvideocapturemodulewebrtcEEAAJPEAU_PinInfoZ)
ModLoad: 000001e8`d0be0000 000001e8`d0be7000   C:\WINDOWS\SYSTEM32\CoreMmRes.dll
ModLoad: 000001e8`d0be0000 000001e8`d0be7000   C:\WINDOWS\SYSTEM32\CoreMmRes.dll
  290     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryPinInfo
    1     0 [  6]             nertc_sdk_d!ILT+146655(?AddRef?$ComRefCountVCaptureSinkFiltervideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::AddRef
    1     0 [  7]               nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc_implwebrtcQEAAXXZ)
   18     0 [  7]               nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    1     0 [  8]                 nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSAHPECHZ)
    9     0 [  8]                 nertc_sdk_d!rtc::AtomicOps::Increment
   21    10 [  7]               nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    9    32 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::AddRef
  295    42 [  5]           nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryPinInfo
   40   346 [  4]         quartz!CFilterGraph::FindOutputPins2
    1     0 [  5]           quartz!guard_dispatch_icall_nop
    1     0 [  5]           nertc_sdk_d!ILT+396005(?EnumPinsCaptureSinkFiltervideocapturemodulewebrtcUEAAJPEAPEAUIEnumPinsZ)
    5     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
    1     0 [  6]             nertc_sdk_d!ILT+604815(??2YAPEAX_KZ)
    4     0 [  6]             nertc_sdk_d!operator new
    1     0 [  7]               nertc_sdk_d!ILT+790005(malloc)
    7     0 [  7]               nertc_sdk_d!malloc
    1     0 [  8]                 nertc_sdk_d!ILT+989705(_malloc_dbg)
   10     0 [  8]                 nertc_sdk_d!_malloc_dbg
    6     0 [  9]                   nertc_sdk_d!heap_alloc_dbg
    1     0 [ 10]                     nertc_sdk_d!ILT+176710(_query_new_mode)
    3     0 [ 10]                     nertc_sdk_d!_query_new_mode
    1     0 [ 11]                       nertc_sdk_d!ILT+71725(?value?$dual_state_globalJ__crt_state_managementQEAAAEAJXZ)
    3     0 [ 11]                       nertc_sdk_d!__crt_state_management::dual_state_global<long>::value
    1     0 [ 12]                         nertc_sdk_d!ILT+753250(?get_current_state_index__crt_state_managementYA_KXZ)
    2     0 [ 12]                         nertc_sdk_d!__crt_state_management::get_current_state_index
    7     3 [ 11]                       nertc_sdk_d!__crt_state_management::dual_state_global<long>::value
    5    11 [ 10]                     nertc_sdk_d!_query_new_mode
    1     0 [ 11]                       nertc_sdk_d!ILT+1127730(??$__crt_interlocked_readJYAJPEDJZ)
    4     0 [ 11]                       nertc_sdk_d!__crt_interlocked_read<long>
    1     0 [ 12]                         nertc_sdk_d!ILT+972445(?__crt_interlocked_read_32YAHPEDHZ)
    9     0 [ 12]                         nertc_sdk_d!__crt_interlocked_read_32
    6    10 [ 11]                       nertc_sdk_d!__crt_interlocked_read<long>
    7    28 [ 10]                     nertc_sdk_d!_query_new_mode
   16    36 [  9]                   nertc_sdk_d!heap_alloc_dbg
    8     0 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 11]                       nertc_sdk_d!ILT+322045(__acrt_lock)
    9     0 [ 11]                       nertc_sdk_d!__acrt_lock
   10     0 [ 12]                         ntdll!RtlEnterCriticalSection
   11    10 [ 11]                       nertc_sdk_d!__acrt_lock
   10    22 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    6     0 [ 11]                       nertc_sdk_d!validate_heap_if_required_nolock
   33    28 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 11]                       nertc_sdk_d!ILT+934125(_guard_xfg_dispatch_icall_nop)
    1     0 [ 11]                       nertc_sdk_d!_guard_xfg_dispatch_icall_nop
    1     0 [ 11]                       nertc_sdk_d!ILT+312535(_guard_dispatch_icall_nop)
    1     0 [ 11]                       nertc_sdk_d!_guard_dispatch_icall_nop
    1     0 [ 11]                       nertc_sdk_d!ILT+762010(_CrtDefaultAllocHook)
    6     0 [ 11]                       nertc_sdk_d!_CrtDefaultAllocHook
   50    39 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
   16     0 [ 11]                       nertc_sdk_d!is_block_type_valid
   60    55 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
   22     0 [ 11]                       ntdll!RtlAllocateHeap
   61     0 [ 11]                       ntdll!RtlpAllocateHeapInternal
   40     0 [ 12]                         ntdll!RtlpAllocateHeap
   17     0 [ 13]                           ntdll!RtlDebugAllocateHeap
   12     0 [ 14]                             ntdll!RtlpCheckHeapSignature
   41    12 [ 13]                           ntdll!RtlDebugAllocateHeap
   10     0 [ 14]                             ntdll!RtlEnterCriticalSection
   47    22 [ 13]                           ntdll!RtlDebugAllocateHeap
   18     0 [ 14]                             ntdll!RtlpValidateHeap
   14     0 [ 15]                               ntdll!RtlpValidateHeapHeaders
   37    14 [ 14]                             ntdll!RtlpValidateHeap
   51    73 [ 13]                           ntdll!RtlDebugAllocateHeap
   22     0 [ 14]                             ntdll!RtlAllocateHeap
   61     0 [ 14]                             ntdll!RtlpAllocateHeapInternal
  341     0 [ 15]                               ntdll!RtlpAllocateHeap
   50     0 [ 16]                                 ntdll!RtlCompareMemoryUlong
  452    50 [ 15]                               ntdll!RtlpAllocateHeap
   16     0 [ 16]                                 ntdll!RtlpFindEntry
  121     0 [ 17]                                   ntdll!RtlpHeapFindListLookupEntry
   23   121 [ 16]                                 ntdll!RtlpFindEntry
  498   194 [ 15]                               ntdll!RtlpAllocateHeap
   28     0 [ 16]                                 ntdll!RtlpHeapAddListEntry
   30     0 [ 17]                                   ntdll!RtlpHeapListCompare
   42    30 [ 16]                                 ntdll!RtlpHeapAddListEntry
  573   266 [ 15]                               ntdll!RtlpAllocateHeap
    7     0 [ 16]                                 ntdll!RtlpGetExtraStuffPointer
  612   273 [ 15]                               ntdll!RtlpAllocateHeap
   88   885 [ 14]                             ntdll!RtlpAllocateHeapInternal
   56  1068 [ 13]                           ntdll!RtlDebugAllocateHeap
   14     0 [ 14]                             ntdll!RtlpValidateHeapHeaders
   75  1082 [ 13]                           ntdll!RtlDebugAllocateHeap
    7     0 [ 14]                             ntdll!RtlpGetExtraStuffPointer
  103  1089 [ 13]                           ntdll!RtlDebugAllocateHeap
   34     0 [ 14]                             ntdll!RtlLeaveCriticalSection
   67     0 [ 15]                               ntdll!RtlpWakeByAddress
    6     0 [ 16]                                 ntdll!NtAlertThreadByThreadId
>> More than one level popped 15 -> 15
   80     6 [ 15]                               ntdll!RtlpWakeByAddress
   41    86 [ 14]                             ntdll!RtlLeaveCriticalSection
  113  1216 [ 13]                           ntdll!RtlDebugAllocateHeap
   50  1329 [ 12]                         ntdll!RtlpAllocateHeap
   88  1379 [ 11]                       ntdll!RtlpAllocateHeapInternal
  121  1544 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 11]                       nertc_sdk_d!ILT+338925(memset)
    6     0 [ 11]                       nertc_sdk_d!memset
    9     0 [ 11]                       nertc_sdk_d!MsetTab
  123  1560 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    4     0 [ 11]                       nertc_sdk_d!block_from_header
  128  1564 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 11]                       nertc_sdk_d!ILT+338925(memset)
    6     0 [ 11]                       nertc_sdk_d!memset
    9     0 [ 11]                       nertc_sdk_d!MsetTab
  130  1580 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    4     0 [ 11]                       nertc_sdk_d!block_from_header
  134  1584 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 11]                       nertc_sdk_d!ILT+338925(memset)
   27     0 [ 11]                       nertc_sdk_d!memset
  136  1612 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    4     0 [ 11]                       nertc_sdk_d!block_from_header
  139  1616 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 11]                       nertc_sdk_d!ILT+1131200(__acrt_unlock)
    9     0 [ 11]                       nertc_sdk_d!__acrt_unlock
   19     0 [ 12]                         ntdll!RtlLeaveCriticalSection
   11    19 [ 11]                       nertc_sdk_d!__acrt_unlock
  142  1647 [ 10]                     nertc_sdk_d!heap_alloc_dbg_internal
   23  1825 [  9]                   nertc_sdk_d!heap_alloc_dbg
   12  1848 [  8]                 nertc_sdk_d!_malloc_dbg
    9  1861 [  7]               nertc_sdk_d!malloc
   11  1871 [  6]             nertc_sdk_d!operator new
   12  1883 [  5]           nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
    1     0 [  6]             nertc_sdk_d!ILT+990975(?get?$scoped_refptrV?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcrtcQEBAPEAV?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcXZ)
    4     0 [  6]             nertc_sdk_d!rtc::scoped_refptr<webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin> >::get
   16  1888 [  5]           nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
    5     0 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins><webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::
    1     0 [  7]               nertc_sdk_d!ILT+96960(??$forwardPEAV?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcstdYA$$QEAPEAV?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcAEAPEAV123Z)
    3     0 [  7]               nertc_sdk_d!std::forward<webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin> *>
   16     4 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins><webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::
    5     0 [  7]               nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::EnumPins
    1     0 [  8]                 nertc_sdk_d!ILT+35960(??0IEnumPinsQEAAXZ)
    4     0 [  8]                 nertc_sdk_d!IEnumPins::IEnumPins
    1     0 [  9]                   nertc_sdk_d!ILT+11215(??0IUnknownQEAAXZ)
    5     0 [  9]                   nertc_sdk_d!IUnknown::IUnknown
    7     6 [  8]                 nertc_sdk_d!IEnumPins::IEnumPins
   14    14 [  7]               nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::EnumPins
    1     0 [  8]                 nertc_sdk_d!ILT+348045(??0?$scoped_refptrUIPinrtcQEAAPEAUIPinZ)
   15     0 [  8]                 nertc_sdk_d!rtc::scoped_refptr<IPin>::scoped_refptr<IPin>
    1     0 [  9]                   nertc_sdk_d!ILT+403800(?AddRef?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcW7EAAKXZ)
    2     0 [  9]                   nertc_sdk_d![thunk]:webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef`adjustor{8}'
    1     0 [  9]                   nertc_sdk_d!ILT+781325(?AddRef?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  9]                   nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef
    1     0 [ 10]                     nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc_implwebrtcQEAAXXZ)
   18     0 [ 10]                     nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    1     0 [ 11]                       nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSAHPECHZ)
    9     0 [ 11]                       nertc_sdk_d!rtc::AtomicOps::Increment
   21    10 [ 10]                     nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    9    32 [  9]                   nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef
   18    45 [  8]                 nertc_sdk_d!rtc::scoped_refptr<IPin>::scoped_refptr<IPin>
   19    78 [  7]               nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::EnumPins
   25   101 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins><webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::
    1     0 [  7]               nertc_sdk_d!ILT+1221030(??0RefCounterwebrtc_implwebrtcQEAAHZ)
    9     0 [  7]               nertc_sdk_d!webrtc::webrtc_impl::RefCounter::RefCounter
   29   111 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins><webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::
   27  2028 [  5]           nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
    6     0 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::AddRef
    1     0 [  7]               nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc_implwebrtcQEAAXXZ)
   18     0 [  7]               nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    1     0 [  8]                 nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSAHPECHZ)
    9     0 [  8]                 nertc_sdk_d!rtc::AtomicOps::Increment
   21    10 [  7]               nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    9    32 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::AddRef
   30  2069 [  5]           nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
   45  2447 [  4]         quartz!CFilterGraph::FindOutputPins2
    1     0 [  5]           quartz!guard_dispatch_icall_nop
    1     0 [  5]           nertc_sdk_d!ILT+609390(?Release?$ComRefCountVCaptureSinkFiltervideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::Release
    1     0 [  6]             nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_implwebrtcQEAA?AW4RefCountReleaseStatusrtcXZ)
   22     0 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
    1     0 [  7]               nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcSAHPECHZ)
    9     0 [  7]               nertc_sdk_d!rtc::AtomicOps::Decrement
   29    10 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
   12    40 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::Release
   54  2501 [  4]         quartz!CFilterGraph::FindOutputPins2
    1     0 [  5]           quartz!guard_dispatch_icall_nop
   21     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::Next
    1     0 [  6]             nertc_sdk_d!ILT+258660(?get?$scoped_refptrUIPinrtcQEBAPEAUIPinXZ)
    4     0 [  6]             nertc_sdk_d!rtc::scoped_refptr<IPin>::get
   35     5 [  5]           nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::Next
    1     0 [  6]             nertc_sdk_d!ILT+403800(?AddRef?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcW7EAAKXZ)
    2     0 [  6]             nertc_sdk_d![thunk]:webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef`adjustor{8}'
    1     0 [  6]             nertc_sdk_d!ILT+781325(?AddRef?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef
    1     0 [  7]               nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc_implwebrtcQEAAXXZ)
   18     0 [  7]               nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    1     0 [  8]                 nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSAHPECHZ)
    9     0 [  8]                 nertc_sdk_d!rtc::AtomicOps::Increment
   21    10 [  7]               nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    9    32 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef
   45    50 [  5]           nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::Next
   66  2597 [  4]         quartz!CFilterGraph::FindOutputPins2
    1     0 [  5]           quartz!guard_dispatch_icall_nop
    1     0 [  5]           nertc_sdk_d!ILT+967675(?QueryDirectionCaptureInputPinvideocapturemodulewebrtcEEAAJPEAW4_PinDirectionZ)
    8     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryDirection
   75  2607 [  4]         quartz!CFilterGraph::FindOutputPins2
    1     0 [  5]           quartz!guard_dispatch_icall_nop
    1     0 [  5]           nertc_sdk_d!ILT+491895(?Release?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcW7EAAKXZ)
    2     0 [  5]           nertc_sdk_d![thunk]:webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release`adjustor{8}'
    1     0 [  5]           nertc_sdk_d!ILT+28620(?Release?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release
    1     0 [  6]             nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_implwebrtcQEAA?AW4RefCountReleaseStatusrtcXZ)
   22     0 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
    1     0 [  7]               nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcSAHPECHZ)
    9     0 [  7]               nertc_sdk_d!rtc::AtomicOps::Decrement
   29    10 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
   12    40 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release
   85  2664 [  4]         quartz!CFilterGraph::FindOutputPins2
    1     0 [  5]           quartz!guard_dispatch_icall_nop
    6     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::Release
    1     0 [  6]             nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_implwebrtcQEAA?AW4RefCountReleaseStatusrtcXZ)
   22     0 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
    1     0 [  7]               nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcSAHPECHZ)
    9     0 [  7]               nertc_sdk_d!rtc::AtomicOps::Decrement
   30    10 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
   22    41 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::Release
    5     0 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::`scalar deleting destructor'
    7     0 [  7]               nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::~ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>
    9     0 [  8]                 nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::~EnumPins
    1     0 [  9]                   nertc_sdk_d!ILT+818255(??1?$scoped_refptrUIPinrtcQEAAXZ)
   11     0 [  9]                   nertc_sdk_d!rtc::scoped_refptr<IPin>::~scoped_refptr<IPin>
    1     0 [ 10]                     nertc_sdk_d!ILT+491895(?Release?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcW7EAAKXZ)
    2     0 [ 10]                     nertc_sdk_d![thunk]:webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release`adjustor{8}'
    1     0 [ 10]                     nertc_sdk_d!ILT+28620(?Release?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [ 10]                     nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release
    1     0 [ 11]                       nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_implwebrtcQEAA?AW4RefCountReleaseStatusrtcXZ)
   22     0 [ 11]                       nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
    1     0 [ 12]                         nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcSAHPECHZ)
    9     0 [ 12]                         nertc_sdk_d!rtc::AtomicOps::Decrement
   29    10 [ 11]                       nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
   12    40 [ 10]                     nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release
   13    56 [  9]                   nertc_sdk_d!rtc::scoped_refptr<IPin>::~scoped_refptr<IPin>
   11    70 [  8]                 nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::~EnumPins
    9    81 [  7]               nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::~ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>
   11    90 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::`scalar deleting destructor'
    1     0 [  7]               nertc_sdk_d!ILT+304785(??3YAXPEAXZ)
    5     0 [  7]               nertc_sdk_d!operator delete
    1     0 [  8]                 nertc_sdk_d!ILT+929290(_free_dbg)
    5     0 [  8]                 nertc_sdk_d!_free_dbg
    1     0 [  9]                   nertc_sdk_d!ILT+322045(__acrt_lock)
    9     0 [  9]                   nertc_sdk_d!__acrt_lock
   10     0 [ 10]                     ntdll!RtlEnterCriticalSection
   11    10 [  9]                   nertc_sdk_d!__acrt_lock
   12    22 [  8]                 nertc_sdk_d!_free_dbg
    4     0 [  9]                   nertc_sdk_d!header_from_block
   20    26 [  8]                 nertc_sdk_d!_free_dbg
    5     0 [  9]                   nertc_sdk_d!free_dbg_nolock
    6     0 [ 10]                     nertc_sdk_d!validate_heap_if_required_nolock
   11     6 [  9]                   nertc_sdk_d!free_dbg_nolock
   10     0 [ 10]                     nertc_sdk_d!is_block_an_aligned_allocation
   24     0 [ 11]                       nertc_sdk_d!check_bytes
   12    24 [ 10]                     nertc_sdk_d!is_block_an_aligned_allocation
   30    42 [  9]                   nertc_sdk_d!free_dbg_nolock
    1     0 [ 10]                     nertc_sdk_d!ILT+934125(_guard_xfg_dispatch_icall_nop)
    1     0 [ 10]                     nertc_sdk_d!_guard_xfg_dispatch_icall_nop
    1     0 [ 10]                     nertc_sdk_d!ILT+312535(_guard_dispatch_icall_nop)
    1     0 [ 10]                     nertc_sdk_d!_guard_dispatch_icall_nop
    1     0 [ 10]                     nertc_sdk_d!ILT+762010(_CrtDefaultAllocHook)
    6     0 [ 10]                     nertc_sdk_d!_CrtDefaultAllocHook
   34    53 [  9]                   nertc_sdk_d!free_dbg_nolock
    1     0 [ 10]                     nertc_sdk_d!ILT+873660(_CrtIsValidHeapPointer)
    6     0 [ 10]                     nertc_sdk_d!_CrtIsValidHeapPointer
    4     0 [ 11]                       nertc_sdk_d!header_from_block
   13     4 [ 10]                     nertc_sdk_d!_CrtIsValidHeapPointer
    1     0 [ 11]                       KERNEL32!HeapValidateStub
    6     0 [ 11]                       KERNELBASE!HeapValidate
   15    11 [ 10]                     nertc_sdk_d!_CrtIsValidHeapPointer
   38    80 [  9]                   nertc_sdk_d!free_dbg_nolock
    4     0 [ 10]                     nertc_sdk_d!header_from_block
   42    84 [  9]                   nertc_sdk_d!free_dbg_nolock
   16     0 [ 10]                     nertc_sdk_d!is_block_type_valid
   55   100 [  9]                   nertc_sdk_d!free_dbg_nolock
   66     0 [ 10]                     nertc_sdk_d!check_bytes
   60   166 [  9]                   nertc_sdk_d!free_dbg_nolock
    4     0 [ 10]                     nertc_sdk_d!block_from_header
   66   170 [  9]                   nertc_sdk_d!free_dbg_nolock
   66     0 [ 10]                     nertc_sdk_d!check_bytes
  111   236 [  9]                   nertc_sdk_d!free_dbg_nolock
    1     0 [ 10]                     nertc_sdk_d!ILT+338925(memset)
   40     0 [ 10]                     nertc_sdk_d!memset
  113   277 [  9]                   nertc_sdk_d!free_dbg_nolock
    1     0 [ 10]                     nertc_sdk_d!ILT+179120(_free_base)
    6     0 [ 10]                     nertc_sdk_d!_free_base
    3     0 [ 11]                       nertc_sdk_d!select_heap
   10     3 [ 10]                     nertc_sdk_d!_free_base
    1     0 [ 11]                       KERNEL32!HeapFreeStub
   21     0 [ 11]                       ntdll!RtlFreeHeap
   41     0 [ 12]                         ntdll!RtlpFreeHeapInternal
   35     0 [ 13]                           ntdll!RtlpFreeHeap
   17     0 [ 14]                             ntdll!RtlDebugFreeHeap
   12     0 [ 15]                               ntdll!RtlpCheckHeapSignature
   27    12 [ 14]                             ntdll!RtlDebugFreeHeap
   10     0 [ 15]                               ntdll!RtlEnterCriticalSection
   33    22 [ 14]                             ntdll!RtlDebugFreeHeap
   18     0 [ 15]                               ntdll!RtlpValidateHeap
   14     0 [ 16]                                 ntdll!RtlpValidateHeapHeaders
   37    14 [ 15]                               ntdll!RtlpValidateHeap
   41    73 [ 14]                             ntdll!RtlDebugFreeHeap
   63     0 [ 15]                               ntdll!RtlpValidateHeapEntry
   51     0 [ 16]                                 ntdll!RtlpCheckBusyBlockTail
   31     0 [ 17]                                   ntdll!RtlCompareMemory
   61    31 [ 16]                                 ntdll!RtlpCheckBusyBlockTail
  121    92 [ 15]                               ntdll!RtlpValidateHeapEntry
   52   286 [ 14]                             ntdll!RtlDebugFreeHeap
   21     0 [ 15]                               ntdll!RtlFreeHeap
   41     0 [ 16]                                 ntdll!RtlpFreeHeapInternal
  187     0 [ 17]                                   ntdll!RtlpFreeHeap
   67     0 [ 18]                                     ntdll!RtlpHeapRemoveListEntry
  204    67 [ 17]                                   ntdll!RtlpFreeHeap
   14     0 [ 18]                                     ntdll!RtlCompareMemoryUlong
  287    81 [ 17]                                   ntdll!RtlpFreeHeap
   16     0 [ 18]                                     ntdll!RtlpFindEntry
  124     0 [ 19]                                       ntdll!RtlpHeapFindListLookupEntry
   23   124 [ 18]                                     ntdll!RtlpFindEntry
  335   228 [ 17]                                   ntdll!RtlpFreeHeap
   43     0 [ 18]                                     ntdll!RtlpHeapAddListEntry
  366   271 [ 17]                                   ntdll!RtlpFreeHeap
   51   637 [ 16]                                 ntdll!RtlpFreeHeapInternal
    8     0 [ 17]                                   ntdll!RtlpHpStackLoggingEnabled
   65   645 [ 16]                                 ntdll!RtlpFreeHeapInternal
   28   710 [ 15]                               ntdll!RtlFreeHeap
   57  1024 [ 14]                             ntdll!RtlDebugFreeHeap
   14     0 [ 15]                               ntdll!RtlpValidateHeapHeaders
   60  1038 [ 14]                             ntdll!RtlDebugFreeHeap
   18     0 [ 15]                               ntdll!RtlpValidateHeap
   14     0 [ 16]                                 ntdll!RtlpValidateHeapHeaders
   37    14 [ 15]                               ntdll!RtlpValidateHeap
   65  1089 [ 14]                             ntdll!RtlDebugFreeHeap
   34     0 [ 15]                               ntdll!RtlLeaveCriticalSection
   67     0 [ 16]                                 ntdll!RtlpWakeByAddress
    6     0 [ 17]                                   ntdll!NtAlertThreadByThreadId
>> More than one level popped 16 -> 16
   80     6 [ 16]                                 ntdll!RtlpWakeByAddress
   41    86 [ 15]                               ntdll!RtlLeaveCriticalSection
   74  1216 [ 14]                             ntdll!RtlDebugFreeHeap
   46  1290 [ 13]                           ntdll!RtlpFreeHeap
   51  1336 [ 12]                         ntdll!RtlpFreeHeapInternal
    8     0 [ 13]                           ntdll!RtlpHpStackLoggingEnabled
   65  1344 [ 12]                         ntdll!RtlpFreeHeapInternal
   28  1409 [ 11]                       ntdll!RtlFreeHeap
   14  1441 [ 10]                     nertc_sdk_d!_free_base
  116  1733 [  9]                   nertc_sdk_d!free_dbg_nolock
   23  1875 [  8]                 nertc_sdk_d!_free_dbg
    1     0 [  9]                   nertc_sdk_d!ILT+1131200(__acrt_unlock)
    9     0 [  9]                   nertc_sdk_d!__acrt_unlock
   19     0 [ 10]                     ntdll!RtlLeaveCriticalSection
   11    19 [  9]                   nertc_sdk_d!__acrt_unlock
   25  1906 [  8]                 nertc_sdk_d!_free_dbg
    7  1932 [  7]               nertc_sdk_d!operator delete
   14  2030 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::`scalar deleting destructor'
   28  2085 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::Release
  100  4778 [  4]         quartz!CFilterGraph::FindOutputPins2
    6     0 [  5]           quartz!_security_check_cookie
  110  4784 [  4]         quartz!CFilterGraph::FindOutputPins2
   28  4894 [  3]       quartz!CFilterGraph::FindOutputPinsHelper
   37  4922 [  2]     quartz!CFilterGraph::IsUpstreamOf
    1     0 [  3]       quartz!operator delete
    1     0 [  3]       quartz!free
    3     0 [  3]       msvcrt!free
   41  4927 [  2]     quartz!CFilterGraph::IsUpstreamOf
    6     0 [  3]       quartz!_security_check_cookie
   51  4933 [  2]     quartz!CFilterGraph::IsUpstreamOf
   37  5365 [  1]   quartz!CFilterGraph::ConnectDirectInternal
    1     0 [  2]     quartz!guard_dispatch_icall_nop
   38     0 [  2]     ksproxy!CKsOutputPin::Connect
   42  5404 [  1]   quartz!CFilterGraph::ConnectDirectInternal
   15     0 [  2]     quartz!CFilterGraph::LogConnectResult
   29     0 [  3]       ntdll!EtwEventEnabled
   21    29 [  2]     quartz!CFilterGraph::LogConnectResult
    6     0 [  3]       quartz!_security_check_cookie
   27    35 [  2]     quartz!CFilterGraph::LogConnectResult
   44  5466 [  1]   quartz!CFilterGraph::ConnectDirectInternal
    9     0 [  2]     quartz!CAutoTimer::~CAutoTimer
    1     0 [  3]       KERNEL32!QueryPerformanceCounterStub
   37     0 [  3]       ntdll!RtlQueryPerformanceCounter
   16    38 [  2]     quartz!CAutoTimer::~CAutoTimer
    8     0 [  3]       quartz!CStats::NewValue
   10     0 [  4]         ntdll!RtlEnterCriticalSection
   40    10 [  3]       quartz!CStats::NewValue
   19     0 [  4]         ntdll!RtlLeaveCriticalSection
   47    29 [  3]       quartz!CStats::NewValue
   19   114 [  2]     quartz!CAutoTimer::~CAutoTimer
   48  5599 [  1]   quartz!CFilterGraph::ConnectDirectInternal
    6     0 [  2]     quartz!_security_check_cookie
   55  5605 [  1]   quartz!CFilterGraph::ConnectDirectInternal
   38  6856 [  0] quartz!CFilterGraph::ConnectDirect
   30     0 [  1]   quartz!CFilterGraph::AttemptDeferredConnections
   40  6886 [  0] quartz!CFilterGraph::ConnectDirect
   12     0 [  1]   quartz!CReconnectList::Passive
   48  6898 [  0] quartz!CFilterGraph::ConnectDirect
    1     0 [  1]   KERNEL32!ReleaseMutex
    9     0 [  1]   KERNELBASE!ReleaseMutex
   51  6908 [  0] quartz!CFilterGraph::ConnectDirect
    7     0 [  1]   quartz!CFilterGraph::NotifyChange
   25     0 [  2]     quartz!CDistributorManager::NotifyGraphChange
    9    25 [  1]   quartz!CFilterGraph::NotifyChange
   63  6942 [  0] quartz!CFilterGraph::ConnectDirect

7005 instructions were executed in 7004 events (0 from other threads)

Function Name                               Invocations MinInst MaxInst AvgInst
KERNEL32!GetCurrentThreadId                           1       3       3       3
KERNEL32!HeapFreeStub                                 1       1       1       1
KERNEL32!HeapValidateStub                             1       1       1       1
KERNEL32!QueryPerformanceCounterStub                  2       1       1       1
KERNEL32!ReleaseMutex                                 1       1       1       1
KERNEL32!WaitForMultipleObjectsEx                     1       1       1       1
KERNEL32!lstrcmpiWStub                                1       1       1       1
KERNELBASE!HeapValidate                               1       6       6       6
KERNELBASE!ReleaseMutex                               1       9       9       9
KERNELBASE!WaitForMultipleObjectsEx                   1      85      85      85
KERNELBASE!lstrcmpiW                                  1     244     244     244
ksproxy!CBaseAllocator::Release                       1       4       4       4
ksproxy!CBaseFilter::QueryFilterInfo                  1      30      30      30
ksproxy!CBasePin::QueryPinInfo                        1      37      37      37
ksproxy!CKsOutputPin::Connect                         1      38      38      38
ksproxy!CUnknown::NonDelegatingRelease                1      11      11      11
ksproxy!guard_dispatch_icall_nop                      1       1       1       1
msvcrt!free                                           1       3       3       3
nertc_sdk_d!IEnumPins::IEnumPins                      1       7       7       7
nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc       5       1       1       1
nertc_sdk_d!ILT+11215(??0IUnknownQEAAXZ)              1       1       1       1
nertc_sdk_d!ILT+1127730(??$__crt_interlocked_re       1       1       1       1
nertc_sdk_d!ILT+1131200(__acrt_unlock)                2       1       1       1
nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcS       5       1       1       1
nertc_sdk_d!ILT+1221030(??0RefCounterwebrtc_imp       1       1       1       1
nertc_sdk_d!ILT+146655(?AddRef?$ComRefCountVCap       2       1       1       1
nertc_sdk_d!ILT+176710(_query_new_mode)               1       1       1       1
nertc_sdk_d!ILT+179120(_free_base)                    1       1       1       1
nertc_sdk_d!ILT+258660(?get?$scoped_refptrUIPin       1       1       1       1
nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_       5       1       1       1
nertc_sdk_d!ILT+28620(?Release?$ComRefCountVCap       2       1       1       1
nertc_sdk_d!ILT+304785(??3YAXPEAXZ)                   1       1       1       1
nertc_sdk_d!ILT+312535(_guard_dispatch_icall_no       2       1       1       1
nertc_sdk_d!ILT+322045(__acrt_lock)                   2       1       1       1
nertc_sdk_d!ILT+338925(memset)                        4       1       1       1
nertc_sdk_d!ILT+348045(??0?$scoped_refptrUIPinr       1       1       1       1
nertc_sdk_d!ILT+35960(??0IEnumPinsQEAAXZ)             1       1       1       1
nertc_sdk_d!ILT+396005(?EnumPinsCaptureSinkFilt       1       1       1       1
nertc_sdk_d!ILT+403800(?AddRef?$ComRefCountVCap       2       1       1       1
nertc_sdk_d!ILT+418995(?QueryFilterInfoCaptureS       1       1       1       1
nertc_sdk_d!ILT+491895(?Release?$ComRefCountVCa       2       1       1       1
nertc_sdk_d!ILT+550225(?QueryInternalConnection       1       1       1       1
nertc_sdk_d!ILT+604815(??2YAPEAX_KZ)                  1       1       1       1
nertc_sdk_d!ILT+609390(?Release?$ComRefCountVCa       2       1       1       1
nertc_sdk_d!ILT+71725(?value?$dual_state_global       1       1       1       1
nertc_sdk_d!ILT+753250(?get_current_state_index       1       1       1       1
nertc_sdk_d!ILT+762010(_CrtDefaultAllocHook)          2       1       1       1
nertc_sdk_d!ILT+781325(?AddRef?$ComRefCountVCap       2       1       1       1
nertc_sdk_d!ILT+790005(malloc)                        1       1       1       1
nertc_sdk_d!ILT+818255(??1?$scoped_refptrUIPinr       1       1       1       1
nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSA       5       1       1       1
nertc_sdk_d!ILT+873660(_CrtIsValidHeapPointer)        1       1       1       1
nertc_sdk_d!ILT+887535(?QueryPinInfoCaptureInpu       2       1       1       1
nertc_sdk_d!ILT+929290(_free_dbg)                     1       1       1       1
nertc_sdk_d!ILT+934125(_guard_xfg_dispatch_ical       2       1       1       1
nertc_sdk_d!ILT+967675(?QueryDirectionCaptureIn       1       1       1       1
nertc_sdk_d!ILT+96960(??$forwardPEAV?$ComRefCou       1       1       1       1
nertc_sdk_d!ILT+972445(?__crt_interlocked_read_       1       1       1       1
nertc_sdk_d!ILT+989705(_malloc_dbg)                   1       1       1       1
nertc_sdk_d!ILT+990975(?get?$scoped_refptrV?$Co       1       1       1       1
nertc_sdk_d!IUnknown::IUnknown                        1       5       5       5
nertc_sdk_d!MsetTab                                   2       9       9       9
nertc_sdk_d![thunk]:webrtc::videocapturemodule:       2       2       2       2
nertc_sdk_d![thunk]:webrtc::videocapturemodule:       2       2       2       2
nertc_sdk_d!_CrtDefaultAllocHook                      2       6       6       6
nertc_sdk_d!_CrtIsValidHeapPointer                    1      15      15      15
nertc_sdk_d!__acrt_lock                               2      11      11      11
nertc_sdk_d!__acrt_unlock                             2      11      11      11
nertc_sdk_d!__crt_interlocked_read<long>              1       6       6       6
nertc_sdk_d!__crt_interlocked_read_32                 1       9       9       9
nertc_sdk_d!__crt_state_management::dual_state_       1       7       7       7
nertc_sdk_d!__crt_state_management::get_current       1       2       2       2
nertc_sdk_d!_free_base                                1      14      14      14
nertc_sdk_d!_free_dbg                                 1      25      25      25
nertc_sdk_d!_guard_dispatch_icall_nop                 2       1       1       1
nertc_sdk_d!_guard_xfg_dispatch_icall_nop             2       1       1       1
nertc_sdk_d!_malloc_dbg                               1      12      12      12
nertc_sdk_d!_query_new_mode                           1       7       7       7
nertc_sdk_d!block_from_header                         4       4       4       4
nertc_sdk_d!check_bytes                               3      24      66      52
nertc_sdk_d!free_dbg_nolock                           1     116     116     116
nertc_sdk_d!header_from_block                         3       4       4       4
nertc_sdk_d!heap_alloc_dbg                            1      23      23      23
nertc_sdk_d!heap_alloc_dbg_internal                   1     142     142     142
nertc_sdk_d!is_block_an_aligned_allocation            1      12      12      12
nertc_sdk_d!is_block_type_valid                       2      16      16      16
nertc_sdk_d!malloc                                    1       9       9       9
nertc_sdk_d!memset                                    4       6      40      19
nertc_sdk_d!operator delete                           1       7       7       7
nertc_sdk_d!operator new                              1      11      11      11
nertc_sdk_d!rtc::AtomicOps::Decrement                 5       9       9       9
nertc_sdk_d!rtc::AtomicOps::Increment                 5       9       9       9
nertc_sdk_d!rtc::scoped_refptr<IPin>::get             1       4       4       4
nertc_sdk_d!rtc::scoped_refptr<IPin>::scoped_re       1      18      18      18
nertc_sdk_d!rtc::scoped_refptr<IPin>::~scoped_r       1      13      13      13
nertc_sdk_d!rtc::scoped_refptr<webrtc::videocap       1       4       4       4
nertc_sdk_d!select_heap                               1       3       3       3
nertc_sdk_d!std::forward<webrtc::videocapturemo       1       3       3       3
nertc_sdk_d!validate_heap_if_required_nolock          2       6       6       6
nertc_sdk_d!webrtc::videocapturemodule::Capture       1       8       8       8
nertc_sdk_d!webrtc::videocapturemodule::Capture       1       5       5       5
nertc_sdk_d!webrtc::videocapturemodule::Capture       2     295     295     295
nertc_sdk_d!webrtc::videocapturemodule::Capture       1      30      30      30
nertc_sdk_d!webrtc::videocapturemodule::Capture       1     287     287     287
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       2       9       9       9
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       2      12      12      12
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       2       9       9       9
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       2      12      12      12
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1       9       9       9
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1      29      29      29
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1      28      28      28
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1      14      14      14
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1       9       9       9
nertc_sdk_d!webrtc::videocapturemodule::`anonym       1      19      19      19
nertc_sdk_d!webrtc::videocapturemodule::`anonym       1      45      45      45
nertc_sdk_d!webrtc::videocapturemodule::`anonym       1      11      11      11
nertc_sdk_d!webrtc::webrtc_impl::RefCounter::De       5      29      30      29
nertc_sdk_d!webrtc::webrtc_impl::RefCounter::In       5      21      21      21
nertc_sdk_d!webrtc::webrtc_impl::RefCounter::Re       1       9       9       9
ntdll!EtwEventEnabled                                 1      29      29      29
ntdll!NtAlertThreadByThreadId                         2       6       6       6
ntdll!RtlAllocateHeap                                 2      22      22      22
ntdll!RtlCompareMemory                                1      31      31      31
ntdll!RtlCompareMemoryUlong                           2      14      50      32
ntdll!RtlDebugAllocateHeap                            1     113     113     113
ntdll!RtlDebugFreeHeap                                1      74      74      74
ntdll!RtlEnterCriticalSection                         6      10      10      10
ntdll!RtlFreeHeap                                     2      28      28      28
ntdll!RtlLeaveCriticalSection                         6      19      41      26
ntdll!RtlQueryPerformanceCounter                      2      37      37      37
ntdll!RtlpAllocateHeap                                2      50     612     331
ntdll!RtlpAllocateHeapInternal                        2      88      88      88
ntdll!RtlpCheckBusyBlockTail                          1      61      61      61
ntdll!RtlpCheckHeapSignature                          2      12      12      12
ntdll!RtlpFindEntry                                   2      23      23      23
ntdll!RtlpFreeHeap                                    2      46     366     206
ntdll!RtlpFreeHeapInternal                            2      65      65      65
ntdll!RtlpGetExtraStuffPointer                        2       7       7       7
ntdll!RtlpHeapAddListEntry                            2      42      43      42
ntdll!RtlpHeapFindListLookupEntry                     2     121     124     122
ntdll!RtlpHeapListCompare                             1      30      30      30
ntdll!RtlpHeapRemoveListEntry                         1      67      67      67
ntdll!RtlpHpStackLoggingEnabled                       2       8       8       8
ntdll!RtlpValidateHeap                                3      37      37      37
ntdll!RtlpValidateHeapEntry                           1     121     121     121
ntdll!RtlpValidateHeapHeaders                         5      14      14      14
ntdll!RtlpWakeByAddress                               2      80      80      80
quartz!CAutoMsgMutex::CAutoMsgMutex                   1      18      18      18
quartz!CAutoTimer::CAutoTimer                         1      37      37      37
quartz!CAutoTimer::~CAutoTimer                        1      19      19      19
quartz!CDistributorManager::NotifyGraphChange         1      25      25      25
quartz!CFilterGraph::AddRef                           1       4       4       4
quartz!CFilterGraph::AttemptDeferredConnections       1      30      30      30
quartz!CFilterGraph::CheckFilterInGraph               2      37      37      37
quartz!CFilterGraph::CheckPinInGraph                  2      35      35      35
quartz!CFilterGraph::ConnectDirect                    1      63      63      63
quartz!CFilterGraph::ConnectDirectInternal            1      55      55      55
quartz!CFilterGraph::FindOutputPins2                  1     110     110     110
quartz!CFilterGraph::FindOutputPinsHelper             1      28      28      28
quartz!CFilterGraph::IsUpstreamOf                     1      51      51      51
quartz!CFilterGraph::LogConnectResult                 1      27      27      27
quartz!CFilterGraph::NotifyChange                     1       9       9       9
quartz!CFilterGraph::Release                          2       4       4       4
quartz!CMsgMutex::Lock                                1      38      38      38
quartz!CReconnectList::Passive                        1      12      12      12
quartz!CStats::Find                                   1      32      32      32
quartz!CStats::NewValue                               1      47      47      47
quartz!CUnknown::NonDelegatingAddRef                  1       6       6       6
quartz!CUnknown::NonDelegatingRelease                 2       9       9       9
quartz!IsEqualObject                                  2      12      12      12
quartz!WaitDispatchingMessages                        1      50      50      50
quartz!_security_check_cookie                         8       6       6       6
quartz!free                                           1       1       1       1
quartz!guard_dispatch_icall_nop                      20       1       1       1
quartz!operator delete                                1       1       1       1

2 system calls were executed

Calls  System Call
    2  ntdll!NtAlertThreadByThreadId
```
</details>

从上面的调用流程分析，可以看出 `quartz::CFilterGraph::ConnectDirect` 的实现中，会回调用户代码中相关的下列实现：

```
nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryPinInfo
nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryInternalConnections
nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::QueryFilterInfo
nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::EnumPins
nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::Next
```

`ksproxy` 模块相关的实现有：

```
ksproxy!CBaseFilter::QueryFilterInfo
ksproxy!CBasePin::QueryPinInfo
ksproxy!CKsOutputPin::Connect 
```

`quartz` 模块相关的重要实现有：

```
quartz!CFilterGraph::AttemptDeferredConnections
quartz!CFilterGraph::CheckFilterInGraph
quartz!CFilterGraph::CheckPinInGraph
quartz!CFilterGraph::ConnectDirect
quartz!CFilterGraph::ConnectDirectInternal
quartz!CFilterGraph::FindOutputPins2
quartz!CFilterGraph::FindOutputPinsHelper
quartz!CFilterGraph::IsUpstreamOf
quartz!CFilterGraph::NotifyChange 
```

大概的流程是 `quartz` 模块的框架代码调用驱动 `ksproxy` 和用户态 `nertc_sdk_d` 的实现，查询 filter 和 pin 相关的接口实现情况。比如检查 pin 的direction，以及其他信息。

## `quartz!CFilterGraph::ConnectDirectInternal`

### 实现

<details>
<summary> 汇编代码 </summary>

```asm
0:005> uf quartz!CFilterGraph::ConnectDirectInternal
quartz!CFilterGraph::ConnectDirectInternal:
00007ffc`2e1511c0 4053            push    rbx
00007ffc`2e1511c2 55              push    rbp
00007ffc`2e1511c3 56              push    rsi
00007ffc`2e1511c4 57              push    rdi
00007ffc`2e1511c5 4156            push    r14
00007ffc`2e1511c7 4881ec50010000  sub     rsp,150h
00007ffc`2e1511ce 488b05339f1300  mov     rax,qword ptr [quartz!_security_cookie (00007ffc`2e28b108)]
00007ffc`2e1511d5 4833c4          xor     rax,rsp
00007ffc`2e1511d8 4889842440010000 mov     qword ptr [rsp+140h],rax
00007ffc`2e1511e0 33db            xor     ebx,ebx
00007ffc`2e1511e2 4d8bf1          mov     r14,r9
00007ffc`2e1511e5 f60594bf160010  test    byte ptr [quartz!Microsoft_Windows_DirectShow_CoreEnableBits (00007ffc`2e2bd180)],10h
00007ffc`2e1511ec 498be8          mov     rbp,r8
00007ffc`2e1511ef 488bfa          mov     rdi,rdx
00007ffc`2e1511f2 895c2440        mov     dword ptr [rsp+40h],ebx
00007ffc`2e1511f6 488bf1          mov     rsi,rcx
00007ffc`2e1511f9 7442            je      quartz!CFilterGraph::ConnectDirectInternal+0x7d (00007ffc`2e15123d)  Branch

quartz!CFilterGraph::ConnectDirectInternal+0x3b:
00007ffc`2e1511fb 498bd0          mov     rdx,r8
00007ffc`2e1511fe 488d8c24d0000000 lea     rcx,[rsp+0D0h]
00007ffc`2e151206 e8e9930e00      call    quartz!CDisp::CDisp (00007ffc`2e23a5f4)
00007ffc`2e15120b 488bd7          mov     rdx,rdi
00007ffc`2e15120e 488d4c2460      lea     rcx,[rsp+60h]
00007ffc`2e151213 488b18          mov     rbx,qword ptr [rax]
00007ffc`2e151216 e8d9930e00      call    quartz!CDisp::CDisp (00007ffc`2e23a5f4)
00007ffc`2e15121b 48895c2430      mov     qword ptr [rsp+30h],rbx
00007ffc`2e151220 4c8bcf          mov     r9,rdi
00007ffc`2e151223 48896c2428      mov     qword ptr [rsp+28h],rbp
00007ffc`2e151228 4c8bc6          mov     r8,rsi
00007ffc`2e15122b 488b00          mov     rax,qword ptr [rax]
00007ffc`2e15122e 4889442420      mov     qword ptr [rsp+20h],rax
00007ffc`2e151233 e84c6f0000      call    quartz!McTemplateU0ppzpz_EventWriteTransfer (00007ffc`2e158184)
00007ffc`2e151238 bb03000000      mov     ebx,3

quartz!CFilterGraph::ConnectDirectInternal+0x7d:
00007ffc`2e15123d f6c302          test    bl,2
00007ffc`2e151240 7417            je      quartz!CFilterGraph::ConnectDirectInternal+0x99 (00007ffc`2e151259)  Branch

quartz!CFilterGraph::ConnectDirectInternal+0x82:
00007ffc`2e151242 488b4c2460      mov     rcx,qword ptr [rsp+60h]
00007ffc`2e151247 488d442468      lea     rax,[rsp+68h]
00007ffc`2e15124c 83e3fd          and     ebx,0FFFFFFFDh
00007ffc`2e15124f 483bc8          cmp     rcx,rax
00007ffc`2e151252 7405            je      quartz!CFilterGraph::ConnectDirectInternal+0x99 (00007ffc`2e151259)  Branch

quartz!CFilterGraph::ConnectDirectInternal+0x94:
00007ffc`2e151254 e81f4dfcff      call    quartz!operator delete (00007ffc`2e115f78)

quartz!CFilterGraph::ConnectDirectInternal+0x99:
00007ffc`2e151259 f6c301          test    bl,1
00007ffc`2e15125c 741a            je      quartz!CFilterGraph::ConnectDirectInternal+0xb8 (00007ffc`2e151278)  Branch

quartz!CFilterGraph::ConnectDirectInternal+0x9e:
00007ffc`2e15125e 488b8c24d0000000 mov     rcx,qword ptr [rsp+0D0h]
00007ffc`2e151266 488d8424d8000000 lea     rax,[rsp+0D8h]
00007ffc`2e15126e 483bc8          cmp     rcx,rax
00007ffc`2e151271 7405            je      quartz!CFilterGraph::ConnectDirectInternal+0xb8 (00007ffc`2e151278)  Branch

quartz!CFilterGraph::ConnectDirectInternal+0xb3:
00007ffc`2e151273 e8004dfcff      call    quartz!operator delete (00007ffc`2e115f78)

quartz!CFilterGraph::ConnectDirectInternal+0xb8:
00007ffc`2e151278 4533c0          xor     r8d,r8d
00007ffc`2e15127b 488d150ef61100  lea     rdx,[quartz!`string' (00007ffc`2e270890)]
00007ffc`2e151282 488d4c2448      lea     rcx,[rsp+48h]
00007ffc`2e151287 e890dcffff      call    quartz!CAutoTimer::CAutoTimer (00007ffc`2e14ef1c)
00007ffc`2e15128c 4c8bc7          mov     r8,rdi
00007ffc`2e15128f 488bd5          mov     rdx,rbp
00007ffc`2e151292 488bce          mov     rcx,rsi
00007ffc`2e151295 e892150100      call    quartz!CFilterGraph::IsUpstreamOf (00007ffc`2e16282c)
00007ffc`2e15129a 85c0            test    eax,eax
00007ffc`2e15129c 7407            je      quartz!CFilterGraph::ConnectDirectInternal+0xe5 (00007ffc`2e1512a5)  Branch

quartz!CFilterGraph::ConnectDirectInternal+0xde:
00007ffc`2e15129e bb31020480      mov     ebx,80040231h
00007ffc`2e1512a3 eb18            jmp     quartz!CFilterGraph::ConnectDirectInternal+0xfd (00007ffc`2e1512bd)  Branch

quartz!CFilterGraph::ConnectDirectInternal+0xe5:
00007ffc`2e1512a5 488b07          mov     rax,qword ptr [rdi]
00007ffc`2e1512a8 4d8bc6          mov     r8,r14
00007ffc`2e1512ab 488bd5          mov     rdx,rbp
00007ffc`2e1512ae 488bcf          mov     rcx,rdi
00007ffc`2e1512b1 488b4018        mov     rax,qword ptr [rax+18h]
00007ffc`2e1512b5 ff153def1000    call    qword ptr [quartz!_guard_dispatch_icall_fptr (00007ffc`2e2601f8)]
00007ffc`2e1512bb 8bd8            mov     ebx,eax

quartz!CFilterGraph::ConnectDirectInternal+0xfd:
00007ffc`2e1512bd 448bc3          mov     r8d,ebx
00007ffc`2e1512c0 488bd7          mov     rdx,rdi
00007ffc`2e1512c3 488bce          mov     rcx,rsi
00007ffc`2e1512c6 e8c5280000      call    quartz!CFilterGraph::LogConnectResult (00007ffc`2e153b90)
00007ffc`2e1512cb 488d4c2448      lea     rcx,[rsp+48h]
00007ffc`2e1512d0 e8f3e4ffff      call    quartz!CAutoTimer::~CAutoTimer (00007ffc`2e14f7c8)
00007ffc`2e1512d5 8bc3            mov     eax,ebx
00007ffc`2e1512d7 488b8c2440010000 mov     rcx,qword ptr [rsp+140h]
00007ffc`2e1512df 4833cc          xor     rcx,rsp
00007ffc`2e1512e2 e8c951fcff      call    quartz!_security_check_cookie (00007ffc`2e1164b0)
00007ffc`2e1512e7 4881c450010000  add     rsp,150h
00007ffc`2e1512ee 415e            pop     r14
00007ffc`2e1512f0 5f              pop     rdi
00007ffc`2e1512f1 5e              pop     rsi
00007ffc`2e1512f2 5d              pop     rbp
00007ffc`2e1512f3 5b              pop     rbx
00007ffc`2e1512f4 c3              ret
```

</details>

### wt 跟踪

<details>

<summary> wt 执行结果 </summary>

```
0:025> wt -m nertc_sdk_d -m quartz -m KERNEL32 -l 200
wt -m nertc_sdk_d -m quartz -m KERNEL32 -l 200
Tracing quartz!CFilterGraph::ConnectDirectInternal to return address 00007ffc`2e151146
   25     0 [  0] quartz!CFilterGraph::ConnectDirectInternal
   16     0 [  1]   quartz!CAutoTimer::CAutoTimer
    9     0 [  2]     quartz!CStats::Find
   10     0 [  3]       ntdll!RtlEnterCriticalSection
   19    10 [  2]     quartz!CStats::Find
    1     0 [  3]       KERNEL32!lstrcmpiWStub
  244     0 [  3]       KERNELBASE!lstrcmpiW
   24   255 [  2]     quartz!CStats::Find
   19     0 [  3]       ntdll!RtlLeaveCriticalSection
   32   274 [  2]     quartz!CStats::Find
   23   306 [  1]   quartz!CAutoTimer::CAutoTimer
    1     0 [  2]     KERNEL32!QueryPerformanceCounterStub
   37     0 [  2]     ntdll!RtlQueryPerformanceCounter
   37   344 [  1]   quartz!CAutoTimer::CAutoTimer
   29   381 [  0] quartz!CFilterGraph::ConnectDirectInternal
   23     0 [  1]   quartz!CFilterGraph::IsUpstreamOf
   16     0 [  2]     quartz!CFilterGraph::FindOutputPinsHelper
   22     0 [  3]       quartz!CFilterGraph::FindOutputPins2
    1     0 [  4]         quartz!guard_dispatch_icall_nop
    1     0 [  4]         nertc_sdk_d!ILT+550225(?QueryInternalConnectionsCaptureInputPinvideocapturemodulewebrtcEEAAJPEAPEAUIPinPEAKZ)
    5     0 [  4]         nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryInternalConnections
   32     7 [  3]       quartz!CFilterGraph::FindOutputPins2
    1     0 [  4]         quartz!guard_dispatch_icall_nop
    1     0 [  4]         nertc_sdk_d!ILT+887535(?QueryPinInfoCaptureInputPinvideocapturemodulewebrtcEEAAJPEAU_PinInfoZ)
  290     0 [  4]         nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryPinInfo
    1     0 [  5]           nertc_sdk_d!ILT+146655(?AddRef?$ComRefCountVCaptureSinkFiltervideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::AddRef
    1     0 [  6]             nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc_implwebrtcQEAAXXZ)
   18     0 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    1     0 [  7]               nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSAHPECHZ)
    9     0 [  7]               nertc_sdk_d!rtc::AtomicOps::Increment
   21    10 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    9    32 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::AddRef
  295    42 [  4]         nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryPinInfo
   40   346 [  3]       quartz!CFilterGraph::FindOutputPins2
    1     0 [  4]         quartz!guard_dispatch_icall_nop
    1     0 [  4]         nertc_sdk_d!ILT+396005(?EnumPinsCaptureSinkFiltervideocapturemodulewebrtcUEAAJPEAPEAUIEnumPinsZ)
    5     0 [  4]         nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
    1     0 [  5]           nertc_sdk_d!ILT+604815(??2YAPEAX_KZ)
    4     0 [  5]           nertc_sdk_d!operator new
    1     0 [  6]             nertc_sdk_d!ILT+790005(malloc)
    7     0 [  6]             nertc_sdk_d!malloc
    1     0 [  7]               nertc_sdk_d!ILT+989705(_malloc_dbg)
   10     0 [  7]               nertc_sdk_d!_malloc_dbg
    6     0 [  8]                 nertc_sdk_d!heap_alloc_dbg
    1     0 [  9]                   nertc_sdk_d!ILT+176710(_query_new_mode)
    3     0 [  9]                   nertc_sdk_d!_query_new_mode
    1     0 [ 10]                     nertc_sdk_d!ILT+71725(?value?$dual_state_globalJ__crt_state_managementQEAAAEAJXZ)
    3     0 [ 10]                     nertc_sdk_d!__crt_state_management::dual_state_global<long>::value
    1     0 [ 11]                       nertc_sdk_d!ILT+753250(?get_current_state_index__crt_state_managementYA_KXZ)
    2     0 [ 11]                       nertc_sdk_d!__crt_state_management::get_current_state_index
    7     3 [ 10]                     nertc_sdk_d!__crt_state_management::dual_state_global<long>::value
    5    11 [  9]                   nertc_sdk_d!_query_new_mode
    1     0 [ 10]                     nertc_sdk_d!ILT+1127730(??$__crt_interlocked_readJYAJPEDJZ)
    4     0 [ 10]                     nertc_sdk_d!__crt_interlocked_read<long>
    1     0 [ 11]                       nertc_sdk_d!ILT+972445(?__crt_interlocked_read_32YAHPEDHZ)
    9     0 [ 11]                       nertc_sdk_d!__crt_interlocked_read_32
    6    10 [ 10]                     nertc_sdk_d!__crt_interlocked_read<long>
    7    28 [  9]                   nertc_sdk_d!_query_new_mode
   16    36 [  8]                 nertc_sdk_d!heap_alloc_dbg
    8     0 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 10]                     nertc_sdk_d!ILT+322045(__acrt_lock)
    9     0 [ 10]                     nertc_sdk_d!__acrt_lock
   10     0 [ 11]                       ntdll!RtlEnterCriticalSection
   11    10 [ 10]                     nertc_sdk_d!__acrt_lock
   10    22 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    6     0 [ 10]                     nertc_sdk_d!validate_heap_if_required_nolock
   33    28 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 10]                     nertc_sdk_d!ILT+934125(_guard_xfg_dispatch_icall_nop)
    1     0 [ 10]                     nertc_sdk_d!_guard_xfg_dispatch_icall_nop
    1     0 [ 10]                     nertc_sdk_d!ILT+312535(_guard_dispatch_icall_nop)
    1     0 [ 10]                     nertc_sdk_d!_guard_dispatch_icall_nop
    1     0 [ 10]                     nertc_sdk_d!ILT+762010(_CrtDefaultAllocHook)
    6     0 [ 10]                     nertc_sdk_d!_CrtDefaultAllocHook
   50    39 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
   16     0 [ 10]                     nertc_sdk_d!is_block_type_valid
   60    55 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
   22     0 [ 10]                     ntdll!RtlAllocateHeap
   88     0 [ 10]                     ntdll!RtlpAllocateHeapInternal
>> No match on ret
   88     0 [ 10]                     ntdll!RtlpAllocateHeapInternal
   62     0 [ 10]                     ntdll!RtlDebugAllocateHeap
>> No match on ret
   62     0 [ 10]                     ntdll!RtlDebugAllocateHeap
   10     0 [ 10]                     ntdll!RtlpAllocateHeap
>> No match on ret
   10     0 [ 10]                     ntdll!RtlpAllocateHeap
   27     0 [ 10]                     ntdll!RtlpAllocateHeapInternal
  121   264 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 10]                     nertc_sdk_d!ILT+338925(memset)
    6     0 [ 10]                     nertc_sdk_d!memset
    9     0 [ 10]                     nertc_sdk_d!MsetTab
  123   280 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    4     0 [ 10]                     nertc_sdk_d!block_from_header
  128   284 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 10]                     nertc_sdk_d!ILT+338925(memset)
    6     0 [ 10]                     nertc_sdk_d!memset
    9     0 [ 10]                     nertc_sdk_d!MsetTab
  130   300 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    4     0 [ 10]                     nertc_sdk_d!block_from_header
  134   304 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 10]                     nertc_sdk_d!ILT+338925(memset)
   27     0 [ 10]                     nertc_sdk_d!memset
  136   332 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    4     0 [ 10]                     nertc_sdk_d!block_from_header
  139   336 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
    1     0 [ 10]                     nertc_sdk_d!ILT+1131200(__acrt_unlock)
    9     0 [ 10]                     nertc_sdk_d!__acrt_unlock
   19     0 [ 11]                       ntdll!RtlLeaveCriticalSection
   11    19 [ 10]                     nertc_sdk_d!__acrt_unlock
  142   367 [  9]                   nertc_sdk_d!heap_alloc_dbg_internal
   23   545 [  8]                 nertc_sdk_d!heap_alloc_dbg
   12   568 [  7]               nertc_sdk_d!_malloc_dbg
    9   581 [  6]             nertc_sdk_d!malloc
   11   591 [  5]           nertc_sdk_d!operator new
   12   603 [  4]         nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
    1     0 [  5]           nertc_sdk_d!ILT+990975(?get?$scoped_refptrV?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcrtcQEBAPEAV?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcXZ)
    4     0 [  5]           nertc_sdk_d!rtc::scoped_refptr<webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin> >::get
   16   608 [  4]         nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
    5     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins><webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::
    1     0 [  6]             nertc_sdk_d!ILT+96960(??$forwardPEAV?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcstdYA$$QEAPEAV?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcAEAPEAV123Z)
    3     0 [  6]             nertc_sdk_d!std::forward<webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin> *>
   16     4 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins><webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::
    5     0 [  6]             nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::EnumPins
    1     0 [  7]               nertc_sdk_d!ILT+35960(??0IEnumPinsQEAAXZ)
    4     0 [  7]               nertc_sdk_d!IEnumPins::IEnumPins
    1     0 [  8]                 nertc_sdk_d!ILT+11215(??0IUnknownQEAAXZ)
    5     0 [  8]                 nertc_sdk_d!IUnknown::IUnknown
    7     6 [  7]               nertc_sdk_d!IEnumPins::IEnumPins
   14    14 [  6]             nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::EnumPins
    1     0 [  7]               nertc_sdk_d!ILT+348045(??0?$scoped_refptrUIPinrtcQEAAPEAUIPinZ)
   15     0 [  7]               nertc_sdk_d!rtc::scoped_refptr<IPin>::scoped_refptr<IPin>
    1     0 [  8]                 nertc_sdk_d!ILT+403800(?AddRef?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcW7EAAKXZ)
    2     0 [  8]                 nertc_sdk_d![thunk]:webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef`adjustor{8}'
    1     0 [  8]                 nertc_sdk_d!ILT+781325(?AddRef?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  8]                 nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef
    1     0 [  9]                   nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc_implwebrtcQEAAXXZ)
   18     0 [  9]                   nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    1     0 [ 10]                     nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSAHPECHZ)
    9     0 [ 10]                     nertc_sdk_d!rtc::AtomicOps::Increment
   21    10 [  9]                   nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    9    32 [  8]                 nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef
   18    45 [  7]               nertc_sdk_d!rtc::scoped_refptr<IPin>::scoped_refptr<IPin>
   19    78 [  6]             nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::EnumPins
   25   101 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins><webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::
    1     0 [  6]             nertc_sdk_d!ILT+1221030(??0RefCounterwebrtc_implwebrtcQEAAHZ)
    9     0 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::RefCounter
   29   111 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins><webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::
   27   748 [  4]         nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
    6     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::AddRef
    1     0 [  6]             nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc_implwebrtcQEAAXXZ)
   18     0 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    1     0 [  7]               nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSAHPECHZ)
    9     0 [  7]               nertc_sdk_d!rtc::AtomicOps::Increment
   21    10 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    9    32 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::AddRef
   30   789 [  4]         nertc_sdk_d!webrtc::videocapturemodule::CaptureSinkFilter::EnumPins
   45  1167 [  3]       quartz!CFilterGraph::FindOutputPins2
    1     0 [  4]         quartz!guard_dispatch_icall_nop
    1     0 [  4]         nertc_sdk_d!ILT+609390(?Release?$ComRefCountVCaptureSinkFiltervideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  4]         nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::Release
    1     0 [  5]           nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_implwebrtcQEAA?AW4RefCountReleaseStatusrtcXZ)
   22     0 [  5]           nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
    1     0 [  6]             nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcSAHPECHZ)
    9     0 [  6]             nertc_sdk_d!rtc::AtomicOps::Decrement
   29    10 [  5]           nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
   12    40 [  4]         nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureSinkFilter>::Release
   54  1221 [  3]       quartz!CFilterGraph::FindOutputPins2
    1     0 [  4]         quartz!guard_dispatch_icall_nop
   21     0 [  4]         nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::Next
    1     0 [  5]           nertc_sdk_d!ILT+258660(?get?$scoped_refptrUIPinrtcQEBAPEAUIPinXZ)
    4     0 [  5]           nertc_sdk_d!rtc::scoped_refptr<IPin>::get
   35     5 [  4]         nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::Next
    1     0 [  5]           nertc_sdk_d!ILT+403800(?AddRef?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcW7EAAKXZ)
    2     0 [  5]           nertc_sdk_d![thunk]:webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef`adjustor{8}'
    1     0 [  5]           nertc_sdk_d!ILT+781325(?AddRef?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef
    1     0 [  6]             nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc_implwebrtcQEAAXXZ)
   18     0 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    1     0 [  7]               nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSAHPECHZ)
    9     0 [  7]               nertc_sdk_d!rtc::AtomicOps::Increment
   21    10 [  6]             nertc_sdk_d!webrtc::webrtc_impl::RefCounter::IncRef
    9    32 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::AddRef
ModLoad: 000001e8`d8000000 000001e8`d8007000   C:\WINDOWS\SYSTEM32\CoreMmRes.dll
   45    50 [  4]         nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::Next
   66  1317 [  3]       quartz!CFilterGraph::FindOutputPins2
    1     0 [  4]         quartz!guard_dispatch_icall_nop
    1     0 [  4]         nertc_sdk_d!ILT+967675(?QueryDirectionCaptureInputPinvideocapturemodulewebrtcEEAAJPEAW4_PinDirectionZ)
    8     0 [  4]         nertc_sdk_d!webrtc::videocapturemodule::CaptureInputPin::QueryDirection
   75  1327 [  3]       quartz!CFilterGraph::FindOutputPins2
    1     0 [  4]         quartz!guard_dispatch_icall_nop
    1     0 [  4]         nertc_sdk_d!ILT+491895(?Release?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcW7EAAKXZ)
    2     0 [  4]         nertc_sdk_d![thunk]:webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release`adjustor{8}'
    1     0 [  4]         nertc_sdk_d!ILT+28620(?Release?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  4]         nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release
    1     0 [  5]           nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_implwebrtcQEAA?AW4RefCountReleaseStatusrtcXZ)
ModLoad: 000001e8`d8000000 000001e8`d8007000   C:\WINDOWS\SYSTEM32\CoreMmRes.dll
   22     0 [  5]           nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
    1     0 [  6]             nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcSAHPECHZ)
    9     0 [  6]             nertc_sdk_d!rtc::AtomicOps::Decrement
   29    10 [  5]           nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
   12    40 [  4]         nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release
   85  1384 [  3]       quartz!CFilterGraph::FindOutputPins2
    1     0 [  4]         quartz!guard_dispatch_icall_nop
    6     0 [  4]         nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::Release
    1     0 [  5]           nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_implwebrtcQEAA?AW4RefCountReleaseStatusrtcXZ)
   22     0 [  5]           nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
    1     0 [  6]             nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcSAHPECHZ)
    9     0 [  6]             nertc_sdk_d!rtc::AtomicOps::Decrement
   30    10 [  5]           nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
   22    41 [  4]         nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::Release
    5     0 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::`scalar deleting destructor'
    7     0 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::~ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>
    9     0 [  7]               nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::~EnumPins
    1     0 [  8]                 nertc_sdk_d!ILT+818255(??1?$scoped_refptrUIPinrtcQEAAXZ)
   11     0 [  8]                 nertc_sdk_d!rtc::scoped_refptr<IPin>::~scoped_refptr<IPin>
    1     0 [  9]                   nertc_sdk_d!ILT+491895(?Release?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcW7EAAKXZ)
    2     0 [  9]                   nertc_sdk_d![thunk]:webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release`adjustor{8}'
    1     0 [  9]                   nertc_sdk_d!ILT+28620(?Release?$ComRefCountVCaptureInputPinvideocapturemodulewebrtcvideocapturemodulewebrtcUEAAKXZ)
    6     0 [  9]                   nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release
    1     0 [ 10]                     nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_implwebrtcQEAA?AW4RefCountReleaseStatusrtcXZ)
   22     0 [ 10]                     nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
    1     0 [ 11]                       nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcSAHPECHZ)
    9     0 [ 11]                       nertc_sdk_d!rtc::AtomicOps::Decrement
   29    10 [ 10]                     nertc_sdk_d!webrtc::webrtc_impl::RefCounter::DecRef
   12    40 [  9]                   nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::CaptureInputPin>::Release
   13    56 [  8]                 nertc_sdk_d!rtc::scoped_refptr<IPin>::~scoped_refptr<IPin>
   11    70 [  7]               nertc_sdk_d!webrtc::videocapturemodule::`anonymous namespace'::EnumPins::~EnumPins
    9    81 [  6]             nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::~ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>
   11    90 [  5]           nertc_sdk_d!webrtc::videocapturemodule::ComRefCount<webrtc::videocapturemodule::`anonymous namespace'::EnumPins>::`scalar deleting destructor'
    1     0 [  6]             nertc_sdk_d!ILT+304785(??3YAXPEAXZ)
    5     0 [  6]             nertc_sdk_d!operator delete
    1     0 [  7]               nertc_sdk_d!ILT+929290(_free_dbg)
    5     0 [  7]               nertc_sdk_d!_free_dbg
    1     0 [  8]                 nertc_sdk_d!ILT+322045(__acrt_lock)
    9     0 [  8]                 nertc_sdk_d!__acrt_lock
   10     0 [  9]                   ntdll!RtlEnterCriticalSection
   11    10 [  8]                 nertc_sdk_d!__acrt_lock
   12    22 [  7]               nertc_sdk_d!_free_dbg
    4     0 [  8]                 nertc_sdk_d!header_from_block
   20    26 [  7]               nertc_sdk_d!_free_dbg
    5     0 [  8]                 nertc_sdk_d!free_dbg_nolock
    6     0 [  9]                   nertc_sdk_d!validate_heap_if_required_nolock
   11     6 [  8]                 nertc_sdk_d!free_dbg_nolock
   10     0 [  9]                   nertc_sdk_d!is_block_an_aligned_allocation
   24     0 [ 10]                     nertc_sdk_d!check_bytes
   12    24 [  9]                   nertc_sdk_d!is_block_an_aligned_allocation
   30    42 [  8]                 nertc_sdk_d!free_dbg_nolock
    1     0 [  9]                   nertc_sdk_d!ILT+934125(_guard_xfg_dispatch_icall_nop)
    1     0 [  9]                   nertc_sdk_d!_guard_xfg_dispatch_icall_nop
    1     0 [  9]                   nertc_sdk_d!ILT+312535(_guard_dispatch_icall_nop)
    1     0 [  9]                   nertc_sdk_d!_guard_dispatch_icall_nop
    1     0 [  9]                   nertc_sdk_d!ILT+762010(_CrtDefaultAllocHook)
    6     0 [  9]                   nertc_sdk_d!_CrtDefaultAllocHook
   34    53 [  8]                 nertc_sdk_d!free_dbg_nolock
    1     0 [  9]                   nertc_sdk_d!ILT+873660(_CrtIsValidHeapPointer)
    6     0 [  9]                   nertc_sdk_d!_CrtIsValidHeapPointer
    4     0 [ 10]                     nertc_sdk_d!header_from_block
   13     4 [  9]                   nertc_sdk_d!_CrtIsValidHeapPointer
    1     0 [ 10]                     KERNEL32!HeapValidateStub
    6     0 [ 10]                     KERNELBASE!HeapValidate
   15    11 [  9]                   nertc_sdk_d!_CrtIsValidHeapPointer
   38    80 [  8]                 nertc_sdk_d!free_dbg_nolock
    4     0 [  9]                   nertc_sdk_d!header_from_block
   42    84 [  8]                 nertc_sdk_d!free_dbg_nolock
   16     0 [  9]                   nertc_sdk_d!is_block_type_valid
   55   100 [  8]                 nertc_sdk_d!free_dbg_nolock
   66     0 [  9]                   nertc_sdk_d!check_bytes
   60   166 [  8]                 nertc_sdk_d!free_dbg_nolock
    4     0 [  9]                   nertc_sdk_d!block_from_header
   66   170 [  8]                 nertc_sdk_d!free_dbg_nolock
   66     0 [  9]                   nertc_sdk_d!check_bytes
  111   236 [  8]                 nertc_sdk_d!free_dbg_nolock
    1     0 [  9]                   nertc_sdk_d!ILT+338925(memset)
   40     0 [  9]                   nertc_sdk_d!memset
  113   277 [  8]                 nertc_sdk_d!free_dbg_nolock
    1     0 [  9]                   nertc_sdk_d!ILT+179120(_free_base)
    6     0 [  9]                   nertc_sdk_d!_free_base
    3     0 [ 10]                     nertc_sdk_d!select_heap
   10     3 [  9]                   nertc_sdk_d!_free_base
    1     0 [ 10]                     KERNEL32!HeapFreeStub
WARNING: This break is not a step/trace completion.
The last command has been cleared to prevent
accidental continuation of this unrelated event.
Check the event, location and thread before resuming.
(6ac0.17f8): Break instruction exception - code 80000003 (first chance)
ntdll!RtlFreeHeap+0x51:
00007ffc`7f1747b1 448bc8          mov     r9d,eax
0:003> g
g
   22     0 [ 10]                     ntdll!RtlFreeHeap

2563 instructions were executed in 2562 events (0 from other threads)

Function Name                               Invocations MinInst MaxInst AvgInst
KERNEL32!HeapFreeStub                                 1       1       1       1
KERNEL32!HeapValidateStub                             1       1       1       1
KERNEL32!QueryPerformanceCounterStub                  1       1       1       1
KERNEL32!lstrcmpiWStub                                1       1       1       1
KERNELBASE!HeapValidate                               1       6       6       6
KERNELBASE!lstrcmpiW                                  1     244     244     244
nertc_sdk_d!IEnumPins::IEnumPins                      1       7       7       7
nertc_sdk_d!ILT+1085295(?IncRefRefCounterwebrtc       4       1       1       1
nertc_sdk_d!ILT+11215(??0IUnknownQEAAXZ)              1       1       1       1
nertc_sdk_d!ILT+1127730(??$__crt_interlocked_re       1       1       1       1
nertc_sdk_d!ILT+1131200(__acrt_unlock)                1       1       1       1
nertc_sdk_d!ILT+1160510(?DecrementAtomicOpsrtcS       4       1       1       1
nertc_sdk_d!ILT+1221030(??0RefCounterwebrtc_imp       1       1       1       1
nertc_sdk_d!ILT+146655(?AddRef?$ComRefCountVCap       1       1       1       1
nertc_sdk_d!ILT+176710(_query_new_mode)               1       1       1       1
nertc_sdk_d!ILT+179120(_free_base)                    1       1       1       1
nertc_sdk_d!ILT+258660(?get?$scoped_refptrUIPin       1       1       1       1
nertc_sdk_d!ILT+279840(?DecRefRefCounterwebrtc_       4       1       1       1
nertc_sdk_d!ILT+28620(?Release?$ComRefCountVCap       2       1       1       1
nertc_sdk_d!ILT+304785(??3YAXPEAXZ)                   1       1       1       1
nertc_sdk_d!ILT+312535(_guard_dispatch_icall_no       2       1       1       1
nertc_sdk_d!ILT+322045(__acrt_lock)                   2       1       1       1
nertc_sdk_d!ILT+338925(memset)                        4       1       1       1
nertc_sdk_d!ILT+348045(??0?$scoped_refptrUIPinr       1       1       1       1
nertc_sdk_d!ILT+35960(??0IEnumPinsQEAAXZ)             1       1       1       1
nertc_sdk_d!ILT+396005(?EnumPinsCaptureSinkFilt       1       1       1       1
nertc_sdk_d!ILT+403800(?AddRef?$ComRefCountVCap       2       1       1       1
nertc_sdk_d!ILT+491895(?Release?$ComRefCountVCa       2       1       1       1
nertc_sdk_d!ILT+550225(?QueryInternalConnection       1       1       1       1
nertc_sdk_d!ILT+604815(??2YAPEAX_KZ)                  1       1       1       1
nertc_sdk_d!ILT+609390(?Release?$ComRefCountVCa       1       1       1       1
nertc_sdk_d!ILT+71725(?value?$dual_state_global       1       1       1       1
nertc_sdk_d!ILT+753250(?get_current_state_index       1       1       1       1
nertc_sdk_d!ILT+762010(_CrtDefaultAllocHook)          2       1       1       1
nertc_sdk_d!ILT+781325(?AddRef?$ComRefCountVCap       2       1       1       1
nertc_sdk_d!ILT+790005(malloc)                        1       1       1       1
nertc_sdk_d!ILT+818255(??1?$scoped_refptrUIPinr       1       1       1       1
nertc_sdk_d!ILT+847175(?IncrementAtomicOpsrtcSA       4       1       1       1
nertc_sdk_d!ILT+873660(_CrtIsValidHeapPointer)        1       1       1       1
nertc_sdk_d!ILT+887535(?QueryPinInfoCaptureInpu       1       1       1       1
nertc_sdk_d!ILT+929290(_free_dbg)                     1       1       1       1
nertc_sdk_d!ILT+934125(_guard_xfg_dispatch_ical       2       1       1       1
nertc_sdk_d!ILT+967675(?QueryDirectionCaptureIn       1       1       1       1
nertc_sdk_d!ILT+96960(??$forwardPEAV?$ComRefCou       1       1       1       1
nertc_sdk_d!ILT+972445(?__crt_interlocked_read_       1       1       1       1
nertc_sdk_d!ILT+989705(_malloc_dbg)                   1       1       1       1
nertc_sdk_d!ILT+990975(?get?$scoped_refptrV?$Co       1       1       1       1
nertc_sdk_d!IUnknown::IUnknown                        1       5       5       5
nertc_sdk_d!MsetTab                                   2       9       9       9
nertc_sdk_d![thunk]:webrtc::videocapturemodule:       2       2       2       2
nertc_sdk_d![thunk]:webrtc::videocapturemodule:       2       2       2       2
nertc_sdk_d!_CrtDefaultAllocHook                      2       6       6       6
nertc_sdk_d!_CrtIsValidHeapPointer                    1      15      15      15
nertc_sdk_d!__acrt_lock                               2      11      11      11
nertc_sdk_d!__acrt_unlock                             1      11      11      11
nertc_sdk_d!__crt_interlocked_read<long>              1       6       6       6
nertc_sdk_d!__crt_interlocked_read_32                 1       9       9       9
nertc_sdk_d!__crt_state_management::dual_state_       1       7       7       7
nertc_sdk_d!__crt_state_management::get_current       1       2       2       2
nertc_sdk_d!_free_base                                1      10      10      10
nertc_sdk_d!_free_dbg                                 1      20      20      20
nertc_sdk_d!_guard_dispatch_icall_nop                 2       1       1       1
nertc_sdk_d!_guard_xfg_dispatch_icall_nop             2       1       1       1
nertc_sdk_d!_malloc_dbg                               1      12      12      12
nertc_sdk_d!_query_new_mode                           1       7       7       7
nertc_sdk_d!block_from_header                         4       4       4       4
nertc_sdk_d!check_bytes                               3      24      66      52
nertc_sdk_d!free_dbg_nolock                           1     113     113     113
nertc_sdk_d!header_from_block                         3       4       4       4
nertc_sdk_d!heap_alloc_dbg                            1      23      23      23
nertc_sdk_d!heap_alloc_dbg_internal                   1     142     142     142
nertc_sdk_d!is_block_an_aligned_allocation            1      12      12      12
nertc_sdk_d!is_block_type_valid                       2      16      16      16
nertc_sdk_d!malloc                                    1       9       9       9
nertc_sdk_d!memset                                    4       6      40      19
nertc_sdk_d!operator delete                           1       5       5       5
nertc_sdk_d!operator new                              1      11      11      11
nertc_sdk_d!rtc::AtomicOps::Decrement                 4       9       9       9
nertc_sdk_d!rtc::AtomicOps::Increment                 4       9       9       9
nertc_sdk_d!rtc::scoped_refptr<IPin>::get             1       4       4       4
nertc_sdk_d!rtc::scoped_refptr<IPin>::scoped_re       1      18      18      18
nertc_sdk_d!rtc::scoped_refptr<IPin>::~scoped_r       1      13      13      13
nertc_sdk_d!rtc::scoped_refptr<webrtc::videocap       1       4       4       4
nertc_sdk_d!select_heap                               1       3       3       3
nertc_sdk_d!std::forward<webrtc::videocapturemo       1       3       3       3
nertc_sdk_d!validate_heap_if_required_nolock          2       6       6       6
nertc_sdk_d!webrtc::videocapturemodule::Capture       1       8       8       8
nertc_sdk_d!webrtc::videocapturemodule::Capture       1       5       5       5
nertc_sdk_d!webrtc::videocapturemodule::Capture       1     295     295     295
nertc_sdk_d!webrtc::videocapturemodule::Capture       1      30      30      30
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       2       9       9       9
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       2      12      12      12
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1       9       9       9
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1      12      12      12
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1       9       9       9
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1      29      29      29
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1      22      22      22
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1      11      11      11
nertc_sdk_d!webrtc::videocapturemodule::ComRefC       1       9       9       9
nertc_sdk_d!webrtc::videocapturemodule::`anonym       1      19      19      19
nertc_sdk_d!webrtc::videocapturemodule::`anonym       1      45      45      45
nertc_sdk_d!webrtc::videocapturemodule::`anonym       1      11      11      11
nertc_sdk_d!webrtc::webrtc_impl::RefCounter::De       4      29      30      29
nertc_sdk_d!webrtc::webrtc_impl::RefCounter::In       4      21      21      21
nertc_sdk_d!webrtc::webrtc_impl::RefCounter::Re       1       9       9       9
ntdll!RtlAllocateHeap                                 1      22      22      22
ntdll!RtlDebugAllocateHeap                            1      62      62      62
ntdll!RtlEnterCriticalSection                         3      10      10      10
ntdll!RtlFreeHeap                                     1      22      22      22
ntdll!RtlLeaveCriticalSection                         2      19      19      19
ntdll!RtlQueryPerformanceCounter                      1      37      37      37
ntdll!RtlpAllocateHeap                                1      10      10      10
ntdll!RtlpAllocateHeapInternal                        2      27      88      57
quartz!CAutoTimer::CAutoTimer                         1      37      37      37
quartz!CFilterGraph::ConnectDirectInternal            1      29      29      29
quartz!CFilterGraph::FindOutputPins2                  1      85      85      85
quartz!CFilterGraph::FindOutputPinsHelper             1      16      16      16
quartz!CFilterGraph::IsUpstreamOf                     1      23      23      23
quartz!CStats::Find                                   1      32      32      32
quartz!guard_dispatch_icall_nop                       8       1       1       1

0 system calls were executed
```
</details>