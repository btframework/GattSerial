unit main;

interface

uses
  Forms, Controls, StdCtrls, Classes, wclBluetooth;

type
  TfmMain = class(TForm)
    btConnect: TButton;
    btDisconnect: TButton;
    laValue: TLabel;
    edValue: TEdit;
    btSend: TButton;
    lbLog: TListBox;
    btClear: TButton;
    wclBluetoothManager: TwclBluetoothManager;
    wclGattClient: TwclGattClient;
    wclBluetoothLeBeaconWatcher: TwclBluetoothLeBeaconWatcher;
    procedure edValueKeyPress(Sender: TObject; var Key: Char);
    procedure btClearClick(Sender: TObject);
    procedure btConnectClick(Sender: TObject);
    procedure wclBluetoothLeBeaconWatcherStarted(Sender: TObject);
    procedure wclBluetoothLeBeaconWatcherStopped(Sender: TObject);
    procedure wclBluetoothLeBeaconWatcherAdvertisementUuidFrame(
      Sender: TObject; const Address: Int64; const Timestamp: Int64;
      const Rssi: Shortint; const Uuid: TGUID);
    procedure wclBluetoothManagerAfterOpen(Sender: TObject);
    procedure wclBluetoothManagerClosed(Sender: TObject);
    procedure btDisconnectClick(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure wclGattClientDisconnect(Sender: TObject;
      const Reason: Integer);
    procedure wclGattClientMaxPduSizeChanged(Sender: TObject);
    procedure wclGattClientConnect(Sender: TObject; const Error: Integer);
    procedure wclGattClientCharacteristicChanged(Sender: TObject;
      const Handle: Word; const Value: TwclGattCharacteristicValue);
    procedure btSendClick(Sender: TObject);

  private
    FTxChar: TwclGattCharacteristic;

    procedure Trace(const Str: string); overload;
    procedure Trace(const Str: string; const Error: Integer); overload;
  end;

var
  fmMain: TfmMain;

implementation

uses
  Dialogs, wclErrors, SysUtils, Windows, wclConnections;

{$R *.dfm}

const
  SERIAL_SERVICE_UUID: TGUID = '{6E40FEC1-B5A3-F393-E0A9-E50E24DCCA9E}';

  RX_CHARACTERISTIC_UUID: TGUID = '{6E40FEC2-B5A3-F393-E0A9-E50E24DCCA9E}';
  TX_CHARACTERISTIC_UUID: TGUID = '{6E40FEC3-B5A3-F393-E0A9-E50E24DCCA9E}';

procedure TfmMain.edValueKeyPress(Sender: TObject; var Key: Char);
var
  Valid: Boolean;
begin
  Valid := (Key in ['0'..'9']) or (Key in ['a'..'f']) or (Key in ['A'..'F']) or
    (Key = #08);
  if not Valid then
    Key := #0;
end;

procedure TfmMain.btClearClick(Sender: TObject);
begin
  lbLog.Items.Clear;
end;

procedure TfmMain.btConnectClick(Sender: TObject);
var
  Res: Integer;
  Radio: TwclBluetoothRadio;
begin
  if wclBluetoothManager.Active then
    ShowMessage('Already connected')

  else begin
    btConnect.Enabled := False;
    btDisconnect.Enabled := True;

    Trace('Opening Bluetooth Manager');
    Res := wclBluetoothManager.Open;
    if Res <> WCL_E_SUCCESS then
      Trace('Bluetooth Manager open failed', Res)

    else begin
      Trace('Get working LE radio');
      Res := wclBluetoothManager.GetLeRadio(Radio);
      if Res <> WCL_E_SUCCESS then
        Trace('Get working LE radio failed', Res)

      else begin
        Trace('Start beacon watcher');
        Res := wclBluetoothLeBeaconWatcher.Start(Radio);
        if Res <> WCL_E_SUCCESS then
          Trace('Start Beacon Watcher failed');
      end;

      if Res <> WCL_E_SUCCESS then
        wclBluetoothManager.Close;
    end;
  end;
end;

procedure TfmMain.Trace(const Str: string);
begin
  lbLog.Items.Add(Str);
end;

procedure TfmMain.Trace(const Str: string; const Error: Integer);
begin
  Trace(Str + ': 0x' + IntToHex(Error, 8));
end;

procedure TfmMain.wclBluetoothLeBeaconWatcherStarted(Sender: TObject);
begin
  Trace('Beacon watcher started');
end;

procedure TfmMain.wclBluetoothLeBeaconWatcherStopped(Sender: TObject);
begin
  Trace('Beacon watcher stopped');
end;

procedure TfmMain.wclBluetoothLeBeaconWatcherAdvertisementUuidFrame(
  Sender: TObject; const Address: Int64; const Timestamp: Int64;
  const Rssi: Shortint; const Uuid: TGUID);
var
  Res: Integer;
  Radio: TwclBluetoothRadio;
begin
  // Additionally you can filter device by MAC or somehow else.
  if CompareMem(@Uuid, @SERIAL_SERVICE_UUID, SizeOf(TGUID)) then begin
    Trace('Device found: ' + IntToHex(Address, 12));

    // Get radio here! After stop it will not be available!
    Radio := wclBluetoothLeBeaconWatcher.Radio;
    wclBluetoothLeBeaconWatcher.Stop;

    Trace('Try to connect');
    wclGattClient.Address := Address;
    Res := wclGattClient.Connect(Radio);
    if Res <> WCL_E_SUCCESS then begin
      Trace('Start connecting failed', Res);
      wclBluetoothManager.Close;
    end;
  end;
end;

procedure TfmMain.wclBluetoothManagerAfterOpen(Sender: TObject);
begin
  Trace('Bluetooth Manager opened');
end;

procedure TfmMain.wclBluetoothManagerClosed(Sender: TObject);
begin
  Trace('Bluetooth Manager closed');

  btConnect.Enabled := True;
  btDisconnect.Enabled := False;
  btSend.Enabled := False;
end;

procedure TfmMain.btDisconnectClick(Sender: TObject);
begin
  if not wclBluetoothManager.Active then
    ShowMessage('Not connected')
  else
    wclBluetoothManager.Close;
end;

procedure TfmMain.FormDestroy(Sender: TObject);
begin
  wclBluetoothManager.Close;
end;

procedure TfmMain.wclGattClientDisconnect(Sender: TObject;
  const Reason: Integer);
begin
  Trace('Client disconnected', Reason);
  wclBluetoothManager.Close;
end;

procedure TfmMain.wclGattClientMaxPduSizeChanged(Sender: TObject);
var
  Res: Integer;
  Size: Word;
begin
  Res := wclGattClient.GetMaxPduSize(Size);
  if Res = WCL_E_SUCCESS then
    Trace('PDU size changed: ' + IntToStr(Size))
  else
    Trace('PDU size changed');
end;

procedure TfmMain.wclGattClientConnect(Sender: TObject;
  const Error: Integer);
var
  Res: Integer;
  Uuid: TwclGattUuid;
  Service: TwclGattService;
  RxChar: TwclGattCharacteristic;
begin
  if Error <> WCL_E_SUCCESS then begin
    Trace('Connect failed', Error);
    wclBluetoothManager.Close;

  end else begin
    Trace('Connected');

    Uuid.IsShortUuid := False;

    Trace('Find service');
    Uuid.LongUuid := SERIAL_SERVICE_UUID;
    Res := wclGattClient.FindService(Uuid, Service);
    if Res <> WCL_E_SUCCESS then
      Trace('Unable to find service', Res)

    else begin
      Trace('Find TX characteristic');
      Uuid.LongUuid := TX_CHARACTERISTIC_UUID;
      Res := wclGattClient.FindCharacteristic(Service, Uuid, FTxChar);
      if Res <> WCL_E_SUCCESS then
        Trace('TX characteristic not found', Res)

      else begin
        Trace('Find RX characteristic');
        Uuid.LongUuid := RX_CHARACTERISTIC_UUID;
        Res := wclGattClient.FindCharacteristic(Service, Uuid, RxChar);
        if Res <> WCL_E_SUCCESS then
          Trace('RX charaterisitc not found', Res)

        else begin
          Trace('Subscribe to notifications');
          Res := wclGattClient.SubscribeForNotifications(RxChar);
          if Res <> WCL_E_SUCCESS then
            Trace('Subscribe failed', Res)

          else begin
            Trace('Connection completed');
            btSend.Enabled := True;
          end;
        end;
      end;
    end;

    if Res <> WCL_E_SUCCESS then begin
      Trace('Wrong device');
      wclGattClient.Disconnect;
    end;
  end;
end;

procedure TfmMain.wclGattClientCharacteristicChanged(Sender: TObject;
  const Handle: Word; const Value: TwclGattCharacteristicValue);
var
  Str: string;
  i: Integer;
begin
  Trace('Data received');
  if Length(Value) = 0 then
    Trace('  Empty data')

  else begin
    Str := '';
    for i := 0 to Length(Value) - 1 do
      Str := Str + IntToHex(Value[i], 2);
    Trace('  Value: ' + Str);
  end;
end;

procedure TfmMain.btSendClick(Sender: TObject);
var
  Str: string;
  Value: TwclGattCharacteristicValue;
  i: Integer;
  j: Integer;
  Res: Integer;
begin
  if wclGattClient.State <> csConnected then
    ShowMessage('Not connected')

  else begin
    Str := edValue.Text;
    if Length(Str) = 0 then
      ShowMessage('Value can not be empty')

    else begin
      if Length(Str) mod 2 <> 0 then
        Str := '0' + Str;

      SetLength(Value, Length(Str) div 2);
      i := 1;
      j := 0;
      while i < Length(Str) do begin
        Value[j] := StrToInt('$' + Copy(Str, i, 2));
        Inc(j);
        Inc(i, 2);
      end;

      Res := wclGattClient.WriteCharacteristicValue(FTxChar, Value);
      if Res <> WCL_E_SUCCESS then
        Trace('Send failed', Res)
      else
        Trace('Sent');
    end;
  end;
end;

end.
