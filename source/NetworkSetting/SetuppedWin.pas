unit SetuppedWin;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, _SplitML, Buttons;

type
  TSetupped = class(TForm)
    Msg1Lbl: TLabel;
    Msg2Lbl: TLabel;
    OpenGlobalINIChk: TCheckBox;
    BitBtn1: TBitBtn;
  private
    { Private êÈåæ }
  public
    { Public êÈåæ }
    procedure InitML;
    procedure ShowEXFS(f:boolean);
  end;

var
  Setupped: TSetupped;

implementation

{$R *.dfm}

procedure TSetupped.InitML;
begin
  SetMLLbl(Msg1Lbl);
  SetMLLbl(Msg2Lbl);
  SetMLChk(OpenGlobalINIChk);                 
end;

procedure TSetupped.ShowEXFS(f:boolean);
begin
  Msg2Lbl.Visible:=f;
end;

end.
