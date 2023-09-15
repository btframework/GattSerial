using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using wclCommon;
using wclCommunication;
using wclBluetooth;

namespace GattSerialClient
{
    public partial class fmMain : Form
    {
        private wclBluetoothManager Manager;
        private wclGattClient Client;
        private wclBluetoothLeBeaconWatcher Watcher;

        private Boolean FClosing;
        private wclGattCharacteristic FTxChar;

        private static Guid SERIAL_SERVICE_UUID = new Guid("{6E40FEC1-B5A3-F393-E0A9-E50E24DCCA9E}");
        private static Guid RX_CHARACTERISTIC_UUID = new Guid("{6E40FEC2-B5A3-F393-E0A9-E50E24DCCA9E}");
        private static Guid TX_CHARACTERISTIC_UUID = new Guid("{6E40FEC3-B5A3-F393-E0A9-E50E24DCCA9E}");
        
        private void Trace(String Str)
        {
            lbLog.Items.Add(Str);
        }

        private void Trace(String Str, Int32 Error)
        {
            Trace(Str + ": 0x" + Error.ToString("X8"));
        }

        public fmMain()
        {
            InitializeComponent();
        }

        private void edValue_KeyPress(Object sender, KeyPressEventArgs e)
        {
            Boolean Valid = ((e.KeyChar >= '0' && e.KeyChar <= '9') || (e.KeyChar >= 'a' && e.KeyChar <= 'f') ||
                (e.KeyChar >= 'A' && e.KeyChar <= 'F') || e.KeyChar == 0x08);
            if (!Valid)
                e.KeyChar = (Char)0x00;
        }

        private void btClear_Click(object sender, EventArgs e)
        {
            lbLog.Items.Clear();
        }

        private void btConnect_Click(object sender, EventArgs e)
        {
            if (Manager.Active)
                MessageBox.Show("Already connected");
            else
            {
                FClosing = false;
                btConnect.Enabled = false;
                btDisconnect.Enabled = true;

                Trace("Opening Bluetooth Manager");
                Int32 Res = Manager.Open();
                if (Res != wclErrors.WCL_E_SUCCESS)
                    Trace("Bluetooth Manager open failed", Res);
                else
                {
                    Trace("Get working LE radio");
                    wclBluetoothRadio Radio;
                    Res = Manager.GetLeRadio(out Radio);
                    if (Res != wclErrors.WCL_E_SUCCESS)
                        Trace("Get working LE radio failed", Res);
                    else
                    {
                        Trace("Start beacon watcher");
                        Res = Watcher.Start(Radio);
                        if (Res != wclErrors.WCL_E_SUCCESS)
                            Trace("Start Beacon Watcher failed");
                    }

                    if (Res != wclErrors.WCL_E_SUCCESS)
                        Manager.Close();
                }
            }
        }

        private void fmMain_Load(object sender, EventArgs e)
        {
            Manager = new wclBluetoothManager();
            Manager.AfterOpen += new EventHandler(Manager_AfterOpen);
            Manager.OnClosed += new EventHandler(Manager_OnClosed);

            Client = new wclGattClient();
            Client.OnDisconnect += new wclClientConnectionDisconnectEvent(Client_OnDisconnect);
            Client.OnMaxPduSizeChanged += new EventHandler(Client_OnMaxPduSizeChanged);
            Client.OnConnect += new wclClientConnectionConnectEvent(Client_OnConnect);
            Client.OnCharacteristicChanged += new wclGattCharacteristicChangedEvent(Client_OnCharacteristicChanged);
            
            Watcher = new wclBluetoothLeBeaconWatcher();
            Watcher.OnStarted += new EventHandler(Watcher_OnStarted);
            Watcher.OnStopped += new EventHandler(Watcher_OnStopped);
            Watcher.OnAdvertisementUuidFrame += new wclBluetoothLeAdvertisementUuidFrameEvent(Watcher_OnAdvertisementUuidFrame);
            
            FClosing = false;
        }

        void Client_OnCharacteristicChanged(Object Sender, UInt16 Handle, Byte[] Value)
        {
            Trace("Data received");
            if (Value == null || Value.Length == 0)
                Trace("  Empty data");
            else
            {
                String Str = "";
                foreach (Byte b in Value)
                    Str = Str + b.ToString("X2");
                Trace("  Value: " + Str);
            }
        }

        void Client_OnConnect(Object Sender, Int32 Error)
        {
            if (Error != wclErrors.WCL_E_SUCCESS)
            {
                Trace("Connect failed", Error);
                Manager.Close();
            }
            else
            {
                Trace("Connected");

                wclGattUuid Uuid = new wclGattUuid();
                Uuid.IsShortUuid = false;

                Trace("Find service");
                Uuid.LongUuid = SERIAL_SERVICE_UUID;
                wclGattService? Service = null;
                Int32 Res = Client.FindService(Uuid, out Service);
                if (Res != wclErrors.WCL_E_SUCCESS)
                    Trace("Unable to find service", Res);
                else
                {
                    Trace("Find TX characteristic");
                    Uuid.LongUuid = TX_CHARACTERISTIC_UUID;
                    wclGattCharacteristic? TxChar = null;
                    Res = Client.FindCharacteristic(Service.Value, Uuid, out TxChar);
                    if (Res != wclErrors.WCL_E_SUCCESS)
                        Trace("TX characteristic not found", Res);
                    else
                    {
                        FTxChar = TxChar.Value;

                        Trace("Find RX characteristic");
                        Uuid.LongUuid = RX_CHARACTERISTIC_UUID;
                        wclGattCharacteristic? RxChar = null;
                        Res = Client.FindCharacteristic(Service.Value, Uuid, out RxChar);
                        if (Res != wclErrors.WCL_E_SUCCESS)
                            Trace("RX charaterisitc not found", Res);
                        else
                        {
                            Trace("Subscribe to notifications");
                            Res = Client.SubscribeForNotifications(RxChar.Value);
                            if (Res != wclErrors.WCL_E_SUCCESS)
                                Trace("Subscribe failed", Res);
                            else
                            {
                                Trace("Connection completed");
                                btSend.Enabled = true;
                            }
                        }
                    }
                }

                if (Res != wclErrors.WCL_E_SUCCESS)
                {
                    Trace("Wrong device");
                    Client.Disconnect();
                }
            }
        }

        void Client_OnMaxPduSizeChanged(Object sender, EventArgs e)
        {
            UInt16 Size;
            Int32 Res = Client.GetMaxPduSize(out Size);
            if (Res == wclErrors.WCL_E_SUCCESS)
                Trace("PDU size changed: " + Size.ToString());
            else
                Trace("PDU size changed");
        }

        void Client_OnDisconnect(Object Sender, Int32 Reason)
        {
            Trace("Client disconnected", Reason);
            if (!FClosing)
                Manager.Close();
        }

        void Manager_OnClosed(object sender, EventArgs e)
        {
            Trace("Bluetooth Manager closed");
            
            btConnect.Enabled = true;
            btDisconnect.Enabled = false;
            btSend.Enabled = false;
        }

        void Manager_AfterOpen(Object sender, EventArgs e)
        {
            Trace("Bluetooth Manager opened");
        }

        void Watcher_OnAdvertisementUuidFrame(Object Sender, Int64 Address, Int64 Timestamp, SByte Rssi, Guid Uuid)
        {
            // Additionally you can filter device by MAC or somehow else.
            if (Uuid == SERIAL_SERVICE_UUID)
            {
                Trace("Device found: " + Address.ToString("X12"));

                // Get radio here! After stop it will not be available!
                wclBluetoothRadio Radio = Watcher.Radio;
                Watcher.Stop();

                Trace("Try to connect");
                Client.Address = Address;
                Int32 Res = Client.Connect(Radio);
                if (Res != wclErrors.WCL_E_SUCCESS)
                {
                    Trace("Start connecting failed", Res);
                    Manager.Close();
                }
            }
        }

        void Watcher_OnStopped(Object sender, EventArgs e)
        {
            Trace("Beacon watcher stopped");
        }

        void Watcher_OnStarted(Object sender, EventArgs e)
        {
            Trace("Beacon watcher started");
        }

        private void btDisconnect_Click(Object sender, EventArgs e)
        {
            if (!Manager.Active)
                MessageBox.Show("Not connected");
            else
            {
                FClosing = true;
                Manager.Close();
            }
        }

        private void fmMain_FormClosed(Object sender, FormClosedEventArgs e)
        {
            FClosing = true;
            Manager.Close();
        }

        private void btSend_Click(Object sender, EventArgs e)
        {
            if (Client.State != wclClientState.csConnected)
                MessageBox.Show("Not connected");
            else
            {
                String Str = edValue.Text;
                if (Str.Length == 0)
                    MessageBox.Show("Value can not be empty");
                else
                {
                    if (Str.Length % 2 != 0)
                        Str = "0" + Str;

                    Byte[] Value = new Byte[Str.Length / 2];
                    for (Int32 i = 0; i < Value.Length; i++)
                    {
                        String b = Str.Substring(i * 2, 2);
                        Value[i] = Convert.ToByte(b, 16);
                    }

                    Int32 Res = Client.WriteCharacteristicValue(FTxChar, Value);
                    if (Res != wclErrors.WCL_E_SUCCESS)
                        Trace("Send failed", Res);
                    else
                        Trace("Sent");
                }
            }
        }
    }
}
