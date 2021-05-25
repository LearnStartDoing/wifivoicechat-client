unit _SplitML;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ComCtrls, StdCtrls, ExtCtrls, Buttons, INIFiles;

const MLType_EN=0;
const MLType_JP=1;
const MLType_Limit=2;

procedure SaveMLType;
procedure SetMLType(_MLType:integer);
function GetMLTypeLoaded:boolean;

function GetMLType:integer;

function GetMLStr(str:string):string;
procedure SetMLLbl(var t:TLabel);
procedure SetMLGrp(var t:TGroupBox);
procedure SetMLChk(var t:TCheckBox);
procedure SetMLBtn(var t:TButton);
procedure SetMLBitBtn(var t:TBitBtn);

implementation

var
  MLTypeLoaded:boolean=False;
  MLType:integer=MLType_Limit;

procedure LoadINI;
var
  fini:TINIFile;
  Section:string;
begin
  if MLTypeLoaded=True then exit;

  fini:=TINIFile.Create(ChangeFileExt(Application.ExeName,'.ini'));

  Section:='system';

  MLType:=fini.ReadInteger(Section,'isJP',MLType);

  if MLType<MLType_Limit then begin
    MLTypeLoaded:=True;
    end else begin
    MLType:=MLType_EN;
    MLTypeLoaded:=False;
  end;

  fini.Free;
end;

procedure SaveINI;
var
  fini:TINIFile;
  Section:string;
begin
  if MLTypeLoaded=False then exit;

  fini:=TINIFile.Create(ChangeFileExt(Application.ExeName,'.ini'));

  Section:='system';

  fini.WriteInteger(Section,'isJP',MLType);

  fini.Free;
end;

procedure SaveMLType;
begin
  if MLTypeLoaded=False then begin
    ShowMessage('FatalError: MLType no set.');
    Application.Terminate;
    exit;
  end;

  SaveINI;
end;

procedure SetMLType(_MLType:integer);
begin
  MLType:=_MLType;

  if MLType<MLType_Limit then begin
    MLTypeLoaded:=True;
    end else begin
    MLType:=MLType_EN;
    MLTypeLoaded:=False;
  end;
end;

function GetMLType:integer;
begin
  Result:=MLType;
end;

function GetMLStr(str:string):string;
var
  pos:integer;
begin
  Result:='';

  LoadINI;
  if MLTypeLoaded=False then begin
    ShowMessage('FatalError: MLType no set.');
    Application.Terminate;
    exit;
  end;

  pos:=ansipos('|',str);
  if pos=0 then begin
    ShowMessage('FatalError: GetMLStr('+str+'); is not sepchar.');
    Application.Terminate;
    exit;
  end;

  case MLType of
    MLType_EN: Result:=copy(str,1,pos-1);
    MLType_JP: Result:=copy(str,pos+1,length(str));
    else begin
      ShowMessage('FatalError: MLType='+inttostr(MLType)+' is not defined.');
      Application.Terminate;
      exit;
    end;
  end;
end;

function GetMLTypeLoaded:boolean;
begin
  if MLTypeLoaded=False then LoadINI;
  Result:=MLTypeLoaded;
end;

procedure SetMLLbl(var t:TLabel);
begin
  t.Caption:=GetMLStr(t.Caption);
end;

procedure SetMLGrp(var t:TGroupBox);
begin
  t.Caption:=GetMLStr(t.Caption);
end;

procedure SetMLChk(var t:TCheckBox);
begin
  t.Caption:=GetMLStr(t.Caption);
end;

procedure SetMLBtn(var t:TButton);
begin
  t.Caption:=GetMLStr(t.Caption);
end;

procedure SetMLBitBtn(var t:TBitBtn);
begin
  t.Caption:=GetMLStr(t.Caption);
end;

end.
