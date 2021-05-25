unit _Cons;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, IdUDPBase,_g721lib;

{$WARN UNIT_PLATFORM OFF}
{$WARN SYMBOL_PLATFORM OFF}

const TimeoutSecValue=3*60; // 3min

const PenPacketHeader=$5a;
const PenPacketHeader32=$5a5a5a5a;

type EConStatus=(ConStatus_Null=0,ConStatus_Login=1,ConStatus_Execute=2);

type EConUserLng=(ConUserLng_JPN=0,ConUserLng_ENG=1,ConUserLng_FRE=2,ConUserLng_DEU=3,ConUserLng_ITA=4,ConUserLng_ESP=5);
const TConUserLng_Max=6;

const UserNameSize=10+1;
const UserMsgSize=26+1;

const g721_recvbufsize=g721_BlockBytes*8;

const RecvBufMax=1024*64;

type TCon=record
  Status:EConStatus;
  IP:string;
  Port:integer;
  TimeoutSec:integer;
  RecvBuf:array[0..RecvBufMax] of byte;
  RecvBufSize:integer;
  RecvBufPos:integer;
  UserLng:EConUserLng;
  UserName:array[0..UserNameSize-1] of widechar;
  UserMsg:array[0..UserMsgSize-1] of widechar;
  UserMacAddr:uint64;
  UserOnTime:word;
  g721_state_dec,g721_state_enc:g721_state;
  g721_dec_LastFrameIndex,g721_enc_LastFrameIndex:dword;
  g721_recv_readpos,g721_recv_writepos:integer;
  g721_recvbuf:array[0..g721_recvbufsize-1] of byte;
  g721_recvdelay:integer;
  RoomNum:word;
  Latency:dword;
end;
type PCon = ^TCon;

const ConsMax=32;

var
  Cons:array[0..ConsMax] of TCon;

var
  LogFilename:string;
  LogStrLst:TStringList;

procedure Cons_Init;
procedure Cons_Init_PCon(Con:PCon;_Status:EConStatus;_IP:string;_Port:integer);
procedure Cons_Init_Con(ConIdx:integer;_Status:EConStatus;_IP:string;_Port:integer);

procedure Cons_ProcIntervalSec;

procedure GlobalSendBuf_AddBuf(pbuf:Pointer;bufsize:integer);
procedure GlobalSendBuf_AddBuf32(data:integer);
function GlobalSendBuf_SendAll:boolean;
function GlobalSendBuf_SendTo(IP:string;Port:integer):boolean;

procedure Cons_SendBuf_AddBuf(pbuf:Pointer;bufsize:integer);
procedure Cons_SendBuf_AddBuf32(data:integer);
procedure Cons_SendBuf_SendAll;

function Cons_FindEqualCon(_IP:string;_Port:integer):PCon;
function Cons_FindNullCon:PCon;

procedure Cons_Buf_AddBuf(Con:PCon;pbuf:Pointer;bufsize:integer);
procedure Cons_Buf_DelBuf(Con:PCon;delsize:integer);
function Cons_Buf_GetDataPos(Con:PCon):integer;
procedure Cons_Buf_SetPos(Con:PCon;pos:integer);
function Cons_Buf_GetPos(Con:PCon):integer;
function Cons_Buf_GetRestSize(Con:PCon):integer;
procedure Cons_Buf_GetBuf(Con:PCon;dstbuf:Pointer;dstsize:integer);
function Cons_Buf_GetBuf32(Con:PCon):integer;

implementation

uses MainWin,_PenPacket;

const GSendBufMax=UDPPacketMaxSize;
var
  GSendBuf:array[0..GSendBufMax] of byte;
  GSendBufSize:integer;

const ConsSendBufMax=UDPPacketMaxSize;
var
  ConsSendBuf:array[0..ConsSendBufMax] of byte;
  ConsSendBufSize:integer;

procedure Cons_Init;
var
  idx:integer;
begin
  LogFilename:=ChangeFileExt(Application.ExeName,'.log');

  GSendBufSize:=0;
  ConsSendBufSize:=0;

  for idx:=0 to ConsMax-1 do begin
    Cons_Init_Con(idx,ConStatus_Null,'',0);
  end;

  LogStrLst:=TStringList.Create;
  if FileExists(LogFilename)=True then LogStrLst.LoadFromFile(LogFilename);
end;

procedure Cons_Init_PCon(Con:PCon;_Status:EConStatus;_IP:string;_Port:integer);
begin
  Con.Status:=_Status;

  Con.IP:=_IP;
  Con.Port:=_Port;

  Con.RecvBufSize:=0;
  Con.RecvBufPos:=0;

  Con.UserLng:=ConUserLng_JPN;
  FillMemory(addr(Con.UserName[0]),UserNameSize*2,0);
  FillMemory(addr(Con.UserMsg[0]),UserMsgSize*2,0);
  Con.UserMacAddr:=0;
  Con.UserOnTime:=0;
end;

procedure Cons_Init_Con(ConIdx:integer;_Status:EConStatus;_IP:string;_Port:integer);
begin
  Cons_Init_PCon(@Cons[ConIdx],_Status,_IP,_Port);
end;

// ---------------------------------------------------

procedure Cons_ProcIntervalSec;
var
  idx:integer;
  apply:boolean;
begin
  apply:=False;
  for idx:=0 to ConsMax-1 do begin
    with Cons[idx] do begin
      if Status<>ConStatus_Null then begin
        inc(UserOnTime);
        if TimeoutSec=0 then begin
          if Status=ConStatus_Execute then LogStrLst.Add(formatdatetime('yyyy/mm/dd hh:nn:ss ',now)+format('%s,%s logout. %dsec.',[UserName,UserMsg,UserOnTime]));
          Status:=ConStatus_Null;
          g721_free(@g721_state_dec);
          g721_free(@g721_state_enc);
          apply:=True;
          end else begin
          dec(TimeoutSec);
        end;
      end;
    end;
  end;
  if apply=True then PenPacket_UserList_GlobalSend;
end;

// ---------------------------------------------------

procedure GlobalSendBuf_AddBuf(pbuf:Pointer;bufsize:integer);
var
  pos:integer;
  buf:PByteArray;
  idx:integer;
  msg:string;
begin
  buf:=pbuf;

  pos:=GSendBufSize;
  if GSendBufMax<(pos+bufsize) then begin
    Main.StatusLst.Items.Add(format('GlobalSendBuf_AddBuf overflow. %d,%d',[pos,bufsize]));
    msg:='';
    for idx:=0 to 12-1 do begin
      msg:=msg+inttohex(GSendBuf[idx],2)+',';
    end;
    Main.StatusLst.Items.Add(msg);
    exit;
  end;
  MoveMemory(addr(GSendBuf[pos]),addr(buf[0]),bufsize);
  inc(GSendBufSize,bufsize);
end;

procedure GlobalSendBuf_AddBuf32(data:integer);
begin
  GlobalSendBuf_AddBuf(addr(data),4);
end;

function GlobalSendBuf_SendAll:boolean;
var
  idx:integer;
begin
  if GSendBufSize=0 then begin
    Result:=False;
    exit;
  end;

  for idx:=0 to ConsMax-1 do begin
    with Cons[idx] do begin
      if Status=ConStatus_Execute then begin
        try
          Main.UDPS.SendBuffer(IP,Port,GSendBuf[0],GSendBufSize);
          except else begin
          end;
        end;
      end;
    end;
  end;

  GSendBufSize:=0;

  Result:=True;
end;

function GlobalSendBuf_SendTo(IP:string;Port:integer):boolean;
begin
  if GSendBufSize=0 then begin
    Result:=False;
    exit;
  end;

  try
    Main.UDPS.SendBuffer(IP,Port,GSendBuf[0],GSendBufSize);
    except else begin end;
  end;

  GSendBufSize:=0;

  Result:=True;
end;

// ---------------------------------------------------

procedure Cons_SendBuf_AddBuf(pbuf:Pointer;bufsize:integer);
var
  pos:integer;
  buf:PByteArray;
begin
  buf:=pbuf;

  pos:=ConsSendBufSize;
  if ConsSendBufMax<(pos+bufsize) then begin
    Main.StatusLst.Items.Add(format('Cons_SendBuf_AddBuf overflow. %d,%d',[pos,bufsize]));
    exit;
  end;
  MoveMemory(addr(ConsSendBuf[pos]),addr(buf[0]),bufsize);
  inc(ConsSendBufSize,bufsize);
end;

procedure Cons_SendBuf_AddBuf32(data:integer);
begin
  Cons_SendBuf_AddBuf(addr(data),4);
end;

procedure Cons_SendBuf_SendAll;
var
  idx:integer;
begin
  if ConsSendBufSize=0 then exit;

  for idx:=0 to ConsMax-1 do begin
    with Cons[idx] do begin
      if Status=ConStatus_Execute then begin
        try
          Main.UDPS.SendBuffer(IP,Port,ConsSendBuf[0],ConsSendBufSize);
          except else begin
          end;
        end;
      end;
    end;
  end;

  ConsSendBufSize:=0;
end;

function Cons_SendBuf_SendTo(IP:string;Port:integer):boolean;
begin
  if ConsSendBufSize=0 then begin
    Result:=False;
    exit;
  end;

  try
    Main.UDPS.SendBuffer(IP,Port,ConsSendBuf[0],ConsSendBufSize);
    except else begin end;
  end;

  ConsSendBufSize:=0;

  Result:=True;
end;

// ---------------------------------------------------

function Cons_FindEqualCon(_IP:string;_Port:integer):PCon;
var
  idx:integer;
begin
  for idx:=0 to ConsMax-1 do begin
    with Cons[idx] do begin
      if Status<>ConStatus_Null then begin
        if (IP=_IP) and (Port=_Port) then begin
          Result:=@Cons[idx];
          exit;
        end;
      end;
    end;
  end;

  Result:=nil;
end;

function Cons_FindNullCon:PCon;
var
  idx:integer;
  smpidx:integer;
begin
  for idx:=0 to ConsMax-1 do begin
    if Cons[idx].Status=ConStatus_Null then begin
      with Cons[idx] do begin
        Status:=ConStatus_Null;
        IP:='';
        Port:=0;
        TimeoutSec:=TimeoutSecValue;
        RecvBufSize:=0;
        RecvBufPos:=0;
        UserLng:=ConUserLng_JPN;
        UserName[0]:=widechar(0);
        UserMsg[0]:=widechar(0);
        UserMacAddr:=0;
        UserOnTime:=0;
        g721_init(@g721_state_dec);
        g721_init(@g721_state_enc);
        g721_dec_LastFrameIndex:=0;
        g721_enc_LastFrameIndex:=0;
        g721_recv_readpos:=0;
        g721_recv_writepos:=0;
        for smpidx:=0 to g721_recvbufsize-1 do begin
          g721_recvbuf[smpidx]:=0;
        end;
        g721_recvdelay:=0;
        RoomNum:=0;
        Latency:=2;
      end;
      Result:=@Cons[idx];
      exit;
    end;
  end;

  Result:=nil;
end;

// ---------------------------------------------------

procedure Cons_Buf_AddBuf(Con:PCon;pbuf:Pointer;bufsize:integer);
var
  pos:integer;
  buf:PByteArray;
begin
  if Con.Status=ConStatus_Execute then Con.TimeoutSec:=TimeoutSecValue;

  buf:=pbuf;

  pos:=Con.RecvBufSize;
  if RecvBufMax<(pos+bufsize) then begin
    Main.StatusLst.Items.Add(format('Cons_Buf_AddBuf overflow. %d,%d',[pos,bufsize]));
    exit;
  end;
  CopyMemory(addr(Con.RecvBuf[pos]),addr(buf[0]),bufsize);
  inc(Con.RecvBufSize,bufsize);
end;

procedure Cons_Buf_AddBuf32(Con:PCon;data:integer);
begin
  Cons_Buf_AddBuf(Con,addr(data),4);
end;

procedure Cons_Buf_DelBuf(Con:PCon;delsize:integer);
var
  NewSize:integer;
begin
  NewSize:=Con.RecvBufSize-delsize;
  if NewSize<>0 then MoveMemory(addr(Con.RecvBuf[0]),addr(Con.RecvBuf[delsize]),NewSize);
  Con.RecvBufSize:=NewSize;
end;

function Cons_Buf_GetDataPos(Con:PCon):integer;
var
  headnum:integer;
  idx:integer;
begin
  headnum:=0;

  for idx:=0 to Con.RecvBufSize-1 do begin
    if Con.RecvBuf[idx]=PenPacketHeader then begin
      inc(headnum);
      if headnum=4 then begin
        Result:=idx+1;
        exit;
      end;
    end;
  end;

  Result:=-1;
end;

procedure Cons_Buf_SetPos(Con:PCon;pos:integer);
begin
  Con.RecvBufPos:=pos;
end;

function Cons_Buf_GetPos(Con:PCon):integer;
begin
  Result:=Con.RecvBufPos;
end;

function Cons_Buf_GetRestSize(Con:PCon):integer;
begin
  Result:=Con.RecvBufSize-Con.RecvBufPos;
end;

procedure Cons_Buf_GetBuf(Con:PCon;dstbuf:Pointer;dstsize:integer);
var
  pb:PByteArray;
begin
  pb:=dstbuf;
  CopyMemory(addr(pb[0]),addr(Con.RecvBuf[Con.RecvBufPos]),dstsize);
  inc(Con.RecvBufPos,dstsize);
end;

function Cons_Buf_GetBuf32(Con:PCon):integer;
begin
  Cons_Buf_GetBuf(Con,addr(Result),4);
end;

end.
