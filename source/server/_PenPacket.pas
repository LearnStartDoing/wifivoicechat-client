unit _PenPacket;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, _Cons,ZLib,_g721lib;

{$WARN UNIT_PLATFORM OFF}
{$WARN SYMBOL_PLATFORM OFF}

const ThisProtcolVersion=11;

const UDPPacketMaxSize=1316;

procedure zlibCompress(src:PByteArray;srcsize:integer;var dst:PByteArray;var dstsize:integer);
procedure zlibDecompress(src:PByteArray;srcsize:integer;var dst:PByteArray;var dstsize:integer);

const DSScrWidth=512;
const DSScrHeight=256;

const DSScrBlockSize=4;

var
  DSScrBuf:array[0..DSScrHeight,0..DSScrWidth] of word;

var
  UserMacAddr:uint64;
  UserLng:EConUserLng;
  UserName:array[0..UserNameSize-1] of widechar;
  UserMsg:array[0..UserMsgSize-1] of widechar;

procedure InitDSScr;

type EPenPacketType=(PenPacketType_SetStatus=0,PenPacketType_GetDSScr=1,PenPacketType_SetDSScr=2,PenPacketType_UserList=3,PenPacketType_SetLatency=4,
                     PenPacketType_Line=10,PenPacketType_TickLine=11,PenPacketType_DrawBox=12,PenPacketType_FillBox=13,PenPacketType_Erase=14,
                     PenPacketType_Sendg721=20,
                     PenPacketType_ChangeRoom=30);

type TPenPacket_SetStatus=packed record
  ProtcolVersion:word;
  iStatus:word;
  iUserLng:word;
  UserName:array[0..UserNameSize-1] of widechar;
  UserMsg:array[0..UserMsgSize-1] of widechar;
  dummy0,dummy1,dummy2:word;
  UserMacAddr:uint64;
end;
const TPenPacket_SetStatus_Size=sizeof(TPenPacket_SetStatus);
procedure PenPacket_SetStatus_Proc(var packet:TPenPacket_SetStatus;Con:PCon);

type TPenPacket_GetDSScr=packed record
  y:word;
end;
const TPenPacket_GetDSScr_Size=sizeof(TPenPacket_GetDSScr);
procedure PenPacket_GetDSScr_Proc(var packet:TPenPacket_GetDSScr;Con:PCon);

type TPenPacket_SetDSScr=packed record
  y:word;
  data:array[0..DSScrWidth*DSScrBlockSize-1] of word;
end;
const TPenPacket_SetDSScr_Size=sizeof(TPenPacket_SetDSScr);
procedure PenPacket_SetDSScr_Proc(var packet:TPenPacket_SetDSScr);

const TPenPacket_UserList_UserMax=64;
type TPenPacket_UserList_User=packed record
  iUserLng:word;
  dummy:word;
  UserName:array[0..UserNameSize-1] of widechar;
  UserMsg:array[0..UserMsgSize-1] of widechar;
  UserOnTime:word;
  dummy0,dummy1,dummy2:word;
  UserMacAddr:uint64;
end;

type TPenPacket_UserList=packed record
  UserCount:word;
  dummy0,dummy1,dummy2:word;
  Users:array[0..TPenPacket_UserList_UserMax-1] of TPenPacket_UserList_User;
end;
const TPenPacket_UserList_Size=sizeof(TPenPacket_UserList);
procedure PenPacket_UserList_Proc(var packet:TPenPacket_UserList;strlst:TStrings);
procedure PenPacket_UserList_GlobalSend;

type TPenPacket_SetLatency=packed record
  Latency:dword;
end;
const TPenPacket_SetLatency_Size=sizeof(TPenPacket_SetLatency);
procedure PenPacket_SetLatency_Proc(var packet:TPenPacket_SetLatency;Con:PCon);

type TPenPacket_Line=packed record
  Color:word;
  lastx,lasty,curx,cury:word;
end;
const TPenPacket_Line_Size=sizeof(TPenPacket_Line);
procedure PenPacket_Line_Proc(var packet:TPenPacket_Line);
procedure PenPacket_Line_GlobalSend(var packet:TPenPacket_Line);

type TPenPacket_TickLine=packed record
  Color:word;
  lastx,lasty,curx,cury:word;
end;
const TPenPacket_TickLine_Size=sizeof(TPenPacket_TickLine);
procedure PenPacket_TickLine_Proc(var packet:TPenPacket_TickLine);
procedure PenPacket_TickLine_GlobalSend(var packet:TPenPacket_TickLine);

type TPenPacket_DrawBox=packed record
  Color:word;
  lastx,lasty,curx,cury:word;
end;
const TPenPacket_DrawBox_Size=sizeof(TPenPacket_DrawBox);
procedure PenPacket_DrawBox_Proc(var packet:TPenPacket_DrawBox);
procedure PenPacket_DrawBox_GlobalSend(var packet:TPenPacket_DrawBox);

type TPenPacket_FillBox=packed record
  Color:word;
  lastx,lasty,curx,cury:word;
end;
const TPenPacket_FillBox_Size=sizeof(TPenPacket_FillBox);
procedure PenPacket_FillBox_Proc(var packet:TPenPacket_FillBox);
procedure PenPacket_FillBox_GlobalSend(var packet:TPenPacket_FillBox);

type TPenPacket_Erase=packed record
  curx,cury:word;
end;
const TPenPacket_Erase_Size=sizeof(TPenPacket_Erase);
procedure PenPacket_Erase_Proc(var packet:TPenPacket_Erase);
procedure PenPacket_Erase_GlobalSend(var packet:TPenPacket_Erase);

type TPenPacket_Sendg721=packed record
  FrameIdx:dword;
  BlockData:array[0..g721_BlockBytes-1] of byte;
end;
const TPenPacket_Sendg721_Size=sizeof(TPenPacket_Sendg721);
procedure PenPacket_Sendg721_Proc(var packet:TPenPacket_Sendg721;Con:PCon);
procedure PenPacket_Sendg721_Send(Con:PCon;pbuf:PSmallInt);

type TPenPacket_ChangeRoom=packed record
  RoomNum:word;
end;
const TPenPacket_ChangeRoom_Size=sizeof(TPenPacket_ChangeRoom);
procedure PenPacket_ChangeRoom_Proc(var packet:TPenPacket_ChangeRoom;Con:PCon);

implementation

uses MainWin;

procedure InitDSScr;
var
  x,y:integer;
begin
  for y:=0 to DSScrHeight-1 do begin
    for x:=0 to DSScrWidth-1 do begin
      DSScrBuf[y,x]:=(1 shl 15);
    end;
  end;
end;

procedure zlibCompress(src:PByteArray;srcsize:integer;var dst:PByteArray;var dstsize:integer);
var
  _EncData:PByteArray;
  _EncDataSize:integer;
begin
  ZLib.CompressBuf(src,srcsize,pointer(_EncData),_EncDataSize);

  if (_EncData=nil) or (_EncDataSize=0) then begin
    dstsize:=0;
    exit;
  end;

  dst:=_EncData;
  dstsize:=_EncDataSize;

//  FreeMem(_EncData,_EncDataSize);
end;

procedure zlibDecompress(src:PByteArray;srcsize:integer;var dst:PByteArray;var dstsize:integer);
var
  _EncData:PByteArray;
  _EncDataSize:integer;
begin
  ZLib.DecompressBuf(src,srcsize,0,pointer(_EncData),_EncDataSize);

  if (_EncData=nil) or (_EncDataSize=0) then begin
    dstsize:=0;
    exit;
  end;

  dst:=_EncData;
  dstsize:=_EncDataSize;

//  FreeMem(_EncData,_EncDataSize);
end;

procedure PenPacket_SetStatus_Proc(var packet:TPenPacket_SetStatus;Con:PCon);
var
  apply:boolean;
begin
  if packet.ProtcolVersion<>ThisProtcolVersion then exit;

  apply:=False;
  if (Con.Status=ConStatus_Login) and (EConStatus(packet.iStatus)=ConStatus_Execute) then begin
    apply:=True;
  end;

  Con.Status:=EConStatus(packet.iStatus);
  Con.UserLng:=EConUserLng(packet.iUserLng);
  MoveMemory(addr(Con.UserName[0]),addr(packet.UserName[0]),UserNameSize*2);
  MoveMemory(addr(Con.UserMsg[0]),addr(packet.UserMsg[0]),UserMsgSize*2);
  Con.UserMacAddr:=packet.UserMacAddr;

  if apply=True then begin
    LogStrLst.Add(formatdatetime('yyyy/mm/dd hh:nn:ss ',now)+format('%s,%s login.',[Con.UserName,Con.UserMsg]));
  end;
end;

procedure PenPacket_GetDSScr_Proc(var packet:TPenPacket_GetDSScr;Con:PCon);
var
  y:integer;
  PenPacket_SetDSScr:TPenPacket_SetDSScr;
  zlibbuf:PByteArray;
  zlibbufsize:integer;
begin
  PenPacket_SetDSScr.y:=packet.y;
  for y:=0 to DSScrBlockSize-1 do begin
    MoveMemory(addr(PenPacket_SetDSScr.data[y*DSScrWidth]),addr(DSScrBuf[packet.y+y][0]),DSScrWidth*2);
  end;

  zlibCompress(addr(PenPacket_SetDSScr),TPenPacket_SetDSScr_Size,zlibbuf,zlibbufsize);

  if UDPPacketMaxSize<=((3*4)+zlibbufsize) then begin
    InitDSScr;
    end else begin
    GlobalSendBuf_AddBuf32(PenPacketHeader32);
    GlobalSendBuf_AddBuf32(dword(PenPacketType_SetDSScr));
    GlobalSendBuf_AddBuf32(zlibbufsize);
    GlobalSendBuf_AddBuf(addr(zlibbuf[0]),zlibbufsize);
    GlobalSendBuf_SendTo(Con.IP,Con.Port);
  end;

  FreeMem(zlibbuf,zlibbufsize);
end;

procedure PenPacket_SetDSScr_Proc(var packet:TPenPacket_SetDSScr);
var
  y:integer;
begin
  for y:=0 to DSScrBlockSize-1 do begin
    MoveMemory(addr(DSScrBuf[packet.y+y][0]),addr(packet.data[y*DSScrWidth]),DSScrWidth*2);
  end;
end;

procedure PenPacket_UserList_Proc(var packet:TPenPacket_UserList;strlst:TStrings);
begin
end;

procedure PenPacket_UserList_GlobalSend;
var
  packet:TPenPacket_UserList;
  conidx,UserCount:integer;
  zlibbuf:PByteArray;
  zlibbufsize:integer;
  procedure adduser(var src:TCon;var dst:TPenPacket_UserList_User);
  begin
    dst.iUserLng:=word(src.UserLng);
    MoveMemory(addr(dst.UserName[0]),addr(src.UserName[0]),UserNameSize*2);
    MoveMemory(addr(dst.UserMsg[0]),addr(src.UserMsg[0]),UserMsgSize*2);
    dst.UserOnTime:=src.UserOnTime;
    dst.UserMacAddr:=src.UserMacAddr;
  end;
  procedure deluser(var dst:TPenPacket_UserList_User);
  begin
    dst.iUserLng:=word(0);
    FillMemory(addr(dst.UserName[0]),UserNameSize*2,0);
    FillMemory(addr(dst.UserMsg[0]),UserMsgSize*2,0);
    dst.UserOnTime:=0;
    dst.UserMacAddr:=0;
  end;
begin
  FillMemory(addr(packet),TPenPacket_UserList_Size,0);

  UserCount:=0;
  for conidx:=0 to ConsMax-1 do begin
    with Cons[conidx] do begin
      if Status=ConStatus_Execute then begin
        adduser(Cons[conidx],packet.Users[UserCount]);
        inc(UserCount);
        if UserCount=TPenPacket_UserList_UserMax then break;
      end;
    end;
  end;

  zlibbuf:=nil;
  zlibbufsize:=0;

  while(True) do begin
    if zlibbuf<>nil then begin
      FreeMem(zlibbuf,zlibbufsize);
      zlibbuf:=nil;
      zlibbufsize:=0;
    end;
    packet.UserCount:=UserCount;
    zlibCompress(addr(packet),TPenPacket_UserList_Size,zlibbuf,zlibbufsize);
    if zlibbufsize<UDPPacketMaxSize then begin
      break;
      end else begin
      deluser(packet.Users[UserCount-1]);
      dec(UserCount);
      if UserCount=0 then begin
        zlibbufsize:=0;
        break;
      end;
    end;
  end;

  if zlibbufsize<>0 then begin
    GlobalSendBuf_AddBuf32(PenPacketHeader32);
    GlobalSendBuf_AddBuf32(dword(PenPacketType_UserList));
    GlobalSendBuf_AddBuf32(zlibbufsize);
    GlobalSendBuf_AddBuf(zlibbuf,zlibbufsize);
    GlobalSendBuf_SendAll;
  end;

  FreeMem(zlibbuf,zlibbufsize);
end;

procedure PenPacket_SetLatency_Proc(var packet:TPenPacket_SetLatency;Con:PCon);
begin
  Con.Latency:=packet.Latency;

  GlobalSendBuf_AddBuf32(PenPacketHeader32);
  GlobalSendBuf_AddBuf32(dword(PenPacketType_SetLatency));
  GlobalSendBuf_AddBuf32(TPenPacket_SetLatency_Size);
  GlobalSendBuf_AddBuf(addr(packet),TPenPacket_SetLatency_Size);
  GlobalSendBuf_SendTo(Con.IP,Con.Port);
end;

procedure PenPacket_Line_Proc(var packet:TPenPacket_Line);
var
  col:word;
  x1,y1,x2,y2:integer;
  ys,ye:integer;
  fye:single;
  xs,xe:integer;
  fxe:single;
  px,py:integer;
  fpx,fpy:single;
  xv,yv:integer;
  fxv,fyv:single;
  drawx,drawy:integer;
  function half16(col1,col2:word):word;
  var
    mask:word;
  begin
    mask:=(30 shl 10) or (30 shl 5) or (30 shl 0);
    col1:=(col1 and mask) shr 1;
    col2:=(col2 and mask) shr 1;
    Result:=(col1+col2) or (1 shl 15);
  end;
begin
  col:=packet.Color or (1 shl 15);
  x1:=packet.lastx;
  y1:=packet.lasty;
  x2:=packet.curx;
  y2:=packet.cury;

  if (x1=x2) and (y1=y2) then exit;

  if (x1<0) or (DSScrWidth<=x1) then exit;
  if (y1<0) or (DSScrHeight<=y1) then exit;
  if (x2<0) or (DSScrWidth<=x2) then exit;
  if (y2<0) or (DSScrHeight<=y2) then exit;

  if x1=x2 then begin
    if y1<y2 then begin
      ys:=y1;
      ye:=y2-1;
      end else begin
      ys:=y2+1;
      ye:=y1;
    end;
    for py:=ys to ye do begin
      DSScrBuf[py,x1]:=col;
    end;
    exit;
  end;

  if y1=y2 then begin
    if x1<x2 then begin
      xs:=x1;
      xe:=x2-1;
      end else begin
      xs:=x2+1;
      xe:=x1;
    end;
    for px:=xs to xe do begin
      DSScrBuf[y1,px]:=col;
    end;
    exit;
  end;

  if abs(x2-x1)>abs(y2-y1) then begin
    px:=0;
    fpy:=0;
    xe:=x2-x1;
    fye:=y2-y1;

    if 0<xe then begin
      xv:=1;
      end else begin
      xv:=-1;
    end;
    fyv:=fye/abs(xe);

    while(px<>xe) do begin
      drawx:=x1+px;
      drawy:=y1+trunc(fpy);
      DSScrBuf[drawy+0,drawx]:=col;
      px:=px+xv;
      fpy:=fpy+fyv;
    end;

    end else begin
    fpx:=0;
    py:=0;
    fxe:=x2-x1;
    ye:=y2-y1;

    if(0<ye) then begin
      yv:=1;
      end else begin
      yv:=-1;
    end;
    fxv:=fxe/abs(ye);

    while(py<>ye) do begin
      drawx:=x1+trunc(fpx);
      drawy:=y1+py;
      DSScrBuf[drawy,drawx+0]:=col;
      fpx:=fpx+fxv;
      py:=py+yv;
    end;
  end;
end;

procedure PenPacket_Line_GlobalSend(var packet:TPenPacket_Line);
begin
  Cons_SendBuf_AddBuf32(PenPacketHeader32);
  Cons_SendBuf_AddBuf32(dword(PenPacketType_Line));
  Cons_SendBuf_AddBuf32(TPenPacket_Line_Size);
  Cons_SendBuf_AddBuf(addr(packet),TPenPacket_Line_Size);
end;

procedure PenPacket_TickLine_Proc(var packet:TPenPacket_TickLine);
var
  col:word;
  x1,y1,x2,y2:integer;
  ys,ye:integer;
  fye:single;
  xs,xe:integer;
  fxe:single;
  px,py:integer;
  fpx,fpy:single;
  xv,yv:integer;
  fxv,fyv:single;
  drawx,drawy:integer;
  function half16(col1,col2:word):word;
  var
    mask:word;
  begin
    mask:=(30 shl 10) or (30 shl 5) or (30 shl 0);
    col1:=(col1 and mask) shr 1;
    col2:=(col2 and mask) shr 1;
    Result:=(col1+col2) or (1 shl 15);
  end;
begin
  col:=packet.Color or (1 shl 15);
  x1:=packet.lastx;
  y1:=packet.lasty;
  x2:=packet.curx;
  y2:=packet.cury;

  if (x1=x2) and (y1=y2) then exit;

  if (x1<1) or ((DSScrWidth-1)<=x1) then exit;
  if (y1<1) or ((DSScrHeight-1)<=y1) then exit;
  if (x2<1) or ((DSScrWidth-1)<=x2) then exit;
  if (y2<1) or ((DSScrHeight-1)<=y2) then exit;

  if x1=x2 then begin
    if y1<y2 then begin
      ys:=y1;
      ye:=y2-1;
      end else begin
      ys:=y2+1;
      ye:=y1;
    end;
    for py:=ys to ye do begin
      DSScrBuf[py,x1-1]:=half16(DSScrBuf[py,x1-1],col);
      DSScrBuf[py,x1]:=col;
      DSScrBuf[py,x1+1]:=half16(DSScrBuf[py,x1+1],col);
    end;
    exit;
  end;

  if y1=y2 then begin
    if x1<x2 then begin
      xs:=x1;
      xe:=x2-1;
      end else begin
      xs:=x2+1;
      xe:=x1;
    end;
    for px:=xs to xe do begin
      DSScrBuf[y1-1,px]:=half16(DSScrBuf[y1-1,px],col);
      DSScrBuf[y1,px]:=col;
      DSScrBuf[y1+1,px]:=half16(DSScrBuf[y1+1,px],col);
    end;
    exit;
  end;

  if abs(x2-x1)>abs(y2-y1) then begin
    px:=0;
    fpy:=0;
    xe:=x2-x1;
    fye:=y2-y1;

    if 0<xe then begin
      xv:=1;
      end else begin
      xv:=-1;
    end;
    fyv:=fye/abs(xe);

    while(px<>xe) do begin
      drawx:=x1+px;
      drawy:=y1+trunc(fpy);
      DSScrBuf[drawy-1,drawx]:=half16(DSScrBuf[drawy-1,drawx],col);
      DSScrBuf[drawy+0,drawx]:=col;
      DSScrBuf[drawy+1,drawx]:=half16(DSScrBuf[drawy+1,drawx],col);
      px:=px+xv;
      fpy:=fpy+fyv;
    end;

    end else begin
    fpx:=0;
    py:=0;
    fxe:=x2-x1;
    ye:=y2-y1;

    if(0<ye) then begin
      yv:=1;
      end else begin
      yv:=-1;
    end;
    fxv:=fxe/abs(ye);

    while(py<>ye) do begin
      drawx:=x1+trunc(fpx);
      drawy:=y1+py;
      DSScrBuf[drawy,drawx-1]:=half16(DSScrBuf[drawy,drawx-1],col);
      DSScrBuf[drawy,drawx+0]:=col;
      DSScrBuf[drawy,drawx+1]:=half16(DSScrBuf[drawy,drawx+1],col);
      fpx:=fpx+fxv;
      py:=py+yv;
    end;
  end;
end;

procedure PenPacket_TickLine_GlobalSend(var packet:TPenPacket_TickLine);
begin
  Cons_SendBuf_AddBuf32(PenPacketHeader32);
  Cons_SendBuf_AddBuf32(dword(PenPacketType_TickLine));
  Cons_SendBuf_AddBuf32(TPenPacket_TickLine_Size);
  Cons_SendBuf_AddBuf(addr(packet),TPenPacket_TickLine_Size);
end;

procedure PenPacket_DrawBox_Proc(var packet:TPenPacket_DrawBox);
var
  col:word;
  x1,y1,x2,y2:integer;
  x,y:integer;
  procedure sw(var d0,d1:integer);
  var
    tmp:integer;
  begin
    tmp:=d0;
    d0:=d1;
    d1:=tmp;
  end;
begin
  col:=packet.Color or (1 shl 15);
  x1:=packet.lastx;
  y1:=packet.lasty;
  x2:=packet.curx;
  y2:=packet.cury;

  if (x1=x2) and (y1=y2) then exit;

  if (x1<0) or (DSScrWidth<=x1) then exit;
  if (y1<0) or (DSScrHeight<=y1) then exit;
  if (x2<0) or (DSScrWidth<=x2) then exit;
  if (y2<0) or (DSScrHeight<=y2) then exit;

  if x2<x1 then sw(x1,x2);
  if y2<y1 then sw(y1,y2);

  for x:=x1 to x2-1 do begin
    DSScrBuf[y1,x]:=col;
    DSScrBuf[y2,x]:=col;
  end;
  for y:=y1 to y2-1 do begin
    DSScrBuf[y,x1]:=col;
    DSScrBuf[y,x2]:=col;
  end;
end;

procedure PenPacket_DrawBox_GlobalSend(var packet:TPenPacket_DrawBox);
begin
  Cons_SendBuf_AddBuf32(PenPacketHeader32);
  Cons_SendBuf_AddBuf32(dword(PenPacketType_DrawBox));
  Cons_SendBuf_AddBuf32(TPenPacket_DrawBox_Size);
  Cons_SendBuf_AddBuf(addr(packet),TPenPacket_DrawBox_Size);
end;

procedure PenPacket_FillBox_Proc(var packet:TPenPacket_FillBox);
var
  col:word;
  x1,y1,x2,y2:integer;
  x,y:integer;
  procedure sw(var d0,d1:integer);
  var
    tmp:integer;
  begin
    tmp:=d0;
    d0:=d1;
    d1:=tmp;
  end;
begin
  col:=packet.Color or (1 shl 15);
  x1:=packet.lastx;
  y1:=packet.lasty;
  x2:=packet.curx;
  y2:=packet.cury;

  if (x1=x2) and (y1=y2) then exit;

  if (x1<0) or (DSScrWidth<=x1) then exit;
  if (y1<0) or (DSScrHeight<=y1) then exit;
  if (x2<0) or (DSScrWidth<=x2) then exit;
  if (y2<0) or (DSScrHeight<=y2) then exit;

  if x2<x1 then sw(x1,x2);
  if y2<y1 then sw(y1,y2);

  for y:=y1 to y2-1 do begin
    for x:=x1 to x2-1 do begin
      DSScrBuf[y,x]:=col;
    end;
  end;
end;

procedure PenPacket_FillBox_GlobalSend(var packet:TPenPacket_FillBox);
begin
  Cons_SendBuf_AddBuf32(PenPacketHeader32);
  Cons_SendBuf_AddBuf32(dword(PenPacketType_FillBox));
  Cons_SendBuf_AddBuf32(TPenPacket_FillBox_Size);
  Cons_SendBuf_AddBuf(addr(packet),TPenPacket_FillBox_Size);
end;

procedure PenPacket_Erase_Proc(var packet:TPenPacket_Erase);
var
  x0,y0:integer;
  x,y:integer;
begin
  x0:=packet.curx;
  y0:=packet.cury;

  if (x0<1) or ((DSScrWidth-1)<=x0) then exit;
  if (y0<1) or ((DSScrHeight-1)<=y0) then exit;

  for y:=y0-4 to y0+4-1 do begin
    for x:=x0-4 to x0+4-1 do begin
      if (0<=x) and (x<DSScrWidth) and (0<=y) and (y<DSScrHeight) then begin
        DSScrBuf[y,x]:=0 or (1 shl 15);
      end;
    end;
  end;
end;

procedure PenPacket_Erase_GlobalSend(var packet:TPenPacket_Erase);
begin
  Cons_SendBuf_AddBuf32(PenPacketHeader32);
  Cons_SendBuf_AddBuf32(dword(PenPacketType_Erase));
  Cons_SendBuf_AddBuf32(TPenPacket_Erase_Size);
  Cons_SendBuf_AddBuf(addr(packet),TPenPacket_Erase_Size);
end;

procedure PenPacket_Sendg721_Proc(var packet:TPenPacket_Sendg721;Con:PCon);
begin
{
  if (Con.g721_dec_LastFrameIndex+1)<>packet.FrameIdx then begin
    main.StatusLst.Items.Add(format('%d,%d',[Con.g721_dec_LastFrameIndex+1,packet.FrameIdx]));
  end;
}

  if packet.FrameIdx=0 then begin
    g721_init(@Con.g721_state_dec);
    end else begin
    if packet.FrameIdx=Con.g721_dec_LastFrameIndex then exit;
  end;

  if (Con.g721_dec_LastFrameIndex+1)<packet.FrameIdx then g721_init(@Con.g721_state_dec);
  Con.g721_dec_LastFrameIndex:=packet.FrameIdx;

  MoveMemory(addr(Con.g721_recvbuf[Con.g721_recv_writepos]),addr(packet.BlockData),g721_BlockBytes);
  inc(Con.g721_recv_writepos,g721_BlockBytes);
  if Con.g721_recv_writepos=g721_recvbufsize then Con.g721_recv_writepos:=0;
end;

procedure PenPacket_Sendg721_Send(Con:PCon;pbuf:PSmallInt);
var
  packet:TPenPacket_Sendg721;
begin
  if Con.g721_enc_LastFrameIndex=0 then g721_init(@Con.g721_state_enc);

  packet.FrameIdx:=Con.g721_enc_LastFrameIndex;
  inc(Con.g721_enc_LastFrameIndex);

  g721_blockencode(@Con.g721_state_enc,pbuf,@packet.BlockData[0]);

  GlobalSendBuf_AddBuf32(PenPacketHeader32);
  GlobalSendBuf_AddBuf32(dword(PenPacketType_Sendg721));
  GlobalSendBuf_AddBuf32(TPenPacket_Sendg721_Size);
  GlobalSendBuf_AddBuf(addr(packet),TPenPacket_Sendg721_Size);
  GlobalSendBuf_SendTo(Con.IP,Con.Port);
end;

procedure PenPacket_ChangeRoom_Proc(var packet:TPenPacket_ChangeRoom;Con:PCon);
begin
  case packet.RoomNum of
    060: if Con.UserLng<>ConUserLng_JPN then exit;
    071: if Con.UserLng<>ConUserLng_ENG then exit;
    082: if Con.UserLng<>ConUserLng_FRE then exit;
    093: if Con.UserLng<>ConUserLng_DEU then exit;
    104: if Con.UserLng<>ConUserLng_ITA then exit;
    115: if Con.UserLng<>ConUserLng_ESP then exit;
    else begin end;
  end;

  Con.RoomNum:=packet.RoomNum;

  GlobalSendBuf_AddBuf32(PenPacketHeader32);
  GlobalSendBuf_AddBuf32(dword(PenPacketType_ChangeRoom));
  GlobalSendBuf_AddBuf32(TPenPacket_ChangeRoom_Size);
  GlobalSendBuf_AddBuf(addr(packet),TPenPacket_ChangeRoom_Size);
  GlobalSendBuf_SendTo(Con.IP,Con.Port);
end;

end.
