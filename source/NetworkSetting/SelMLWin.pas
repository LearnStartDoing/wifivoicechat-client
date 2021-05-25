unit SelMLWin;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, _SplitML;

type
  TSelML = class(TForm)
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    ENRadioBtn: TRadioButton;
    JPRadioBtn: TRadioButton;
    Label1: TLabel;
    Label2: TLabel;
    SaveChk: TCheckBox;
    Label3: TLabel;
    procedure BitBtn1Click(Sender: TObject);
    procedure Label3Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private êÈåæ }
  public
    { Public êÈåæ }
  end;

var
  SelML: TSelML;

implementation

{$R *.dfm}

procedure TSelML.BitBtn1Click(Sender: TObject);
begin
  if ENRadioBtn.Checked=True then SetMLType(MLType_EN);
  if JPRadioBtn.Checked=True then SetMLType(MLType_JP);

  if SaveChk.Checked=True then SaveMLType;
end;

procedure TSelML.Label3Click(Sender: TObject);
begin
  SaveChk.Checked:=not SaveChk.Checked;
end;

procedure TSelML.FormCreate(Sender: TObject);
var
  inifn:string;
begin
  inifn:=ChangeFileExt(ExtractFilename(Application.ExeName),'.ini');
  SaveChk.Caption:=format(SaveChk.Caption,[inifn]);
  Label3.Caption:=format(Label3.Caption,[inifn]);

end;

end.
