using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;
using Newtonsoft.Json;
using System.Collections.ObjectModel;




namespace raichumobile2
{
   
    
       
    public partial class MainPage : PhoneApplicationPage
    {
        SocketClient client = new SocketClient();
        public ObservableCollection<ItemViewModel> DeviceItems { get; private set; }
        public List<String> listOfDevices = new List<String>();
        public ObservableCollection<ItemViewModel> connectedDevices { get; private set; }
        public List<String> ListOfConnectedDevices = new List<String>();
        bool connected = false;

        // Constants
        const int ECHO_PORT = 30000;  // The Echo protocol uses port 7 in this sample
        const int QOTD_PORT = 17; // The Quote of the Day (QOTD) protocol uses port 17 in this sample
        // Constructor
        public MainPage()
        {
            
            String SERVER_NAME = "Pkm-center@sjsu.edu";
            String SERVER_IP = "130.65.157.291";
            int SERVER_PORT = 33333;
            InitializeComponent();
            serverConnect();

            // Set the data context of the listbox control to the sample data
            DataContext = App.ViewModel;
            this.Loaded += new RoutedEventHandler(MainPage_Loaded);
           
        }
        public  void serverConnect()
        {
            // Clear the log 
            ClearLog();
            connected = true;
            Device product = new Device();
            DateTime time = new DateTime();
            product.name = deviceNameTextBox.Text;
            product.type = "client";
            product.bootup_time = "2012-12-04 23:17:42";

            string output = JsonConvert.SerializeObject(product);

        
            String connectionString;
            // Make sure we can perform this action with valid data
            if (ValidateRemoteHost() && ValidateInput())
            {
                // Instantiate the SocketClient
              

                // Attempt to connect to the echo server
                string result = client.Connect(ServerAddressTextBox.Text, ECHO_PORT);
                connectionStatusTextBLock.Text = "Connection Status: Online";

                result = client.Send(output);
                Log(result, false);
                result = client.Send("list");
                result = client.Receive();
                //Log(result, false); 
                if (result != null) ;
                {
                    DeviceList devices = JsonConvert.DeserializeObject<DeviceList>(result);
                    Log(devices.deviceListFromServer[0], false);
                    while (devices.deviceListFromServer == null)
                    {
                    }
                    DeviceItems = new ObservableCollection<ItemViewModel>();
                    for (int i = 0; i < devices.deviceListFromServer.Length; i++)
                    {
                        DeviceItems.Add(new ItemViewModel() { LineOne = devices.deviceListFromServer[i], LineTwo = "remote device", LineThree = "" });

                    }
                    allDevicesListBox.ItemsSource = DeviceItems;
                    myDeviceListBox.ItemsSource = connectedDevices;
                    
                }

                // Receive a response from the echo server

                result = client.Receive();

            }
        }
        public partial class Device
        {

            public String name;
            public String type;
            public String bootup_time;



            public Device()
            {
                name = "";
                type = "";
                bootup_time = "";
            }
        }
        public partial class DeviceList
        {
            public String[] deviceListFromServer;
            public DeviceList()
            {

            }
        }

        // Load data for the ViewModel Items
        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            if (!App.ViewModel.IsDataLoaded)
            {
                App.ViewModel.LoadData();
            }
        }
        #region UI Validation
        /// <summary>
        /// Validates the txtInput TextBox
        /// </summary>
        /// <returns>True if the txtInput TextBox contains valid data, False otherwise</returns>
        private bool ValidateInput()
        {
            // txtInput must contain some text
            if (String.IsNullOrWhiteSpace(ServerPortNumberTextBox.Text))
            {
                MessageBox.Show("Please enter some text to echo");
                return false;
            }

            return true;
        }

        /// <summary>
        /// Validates the txtRemoteHost TextBox
        /// </summary>
        /// <returns>True if the txtRemoteHost contains valid data, False otherwise</returns>
        private bool ValidateRemoteHost()
        {
            // The txtRemoteHost must contain some text
            if (String.IsNullOrWhiteSpace(ServerAddressTextBox.Text))
            {
                MessageBox.Show("Please enter a valid host name");
                return false;
            }

            return true;
        }
        #endregion

        #region Logging
        /// <summary>
        /// Log text to the txtOutput TextBox
        /// </summary>
        /// <param name="message">The message to write to the txtOutput TextBox</param>
        /// <param name="isOutgoing">True if the message is an outgoing (client to server) message, False otherwise</param>
        /// <remarks>We differentiate between a message from the client and server 
        /// by prepending each line  with ">>" and "<<" respectively.</remarks>
        private void Log(string message, bool isOutgoing)
        {
            string direction = (isOutgoing) ? ">> " : "<< ";
            txtOutput.Text += Environment.NewLine + direction + message;
        }

        /// <summary>
        /// Clears the txtOutput TextBox
        /// </summary>
        private void ClearLog()
        {
            txtOutput.Text = String.Empty;
        }
        #endregion

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            // Clear the log 
            ClearLog();

            Device product = new Device();
            DateTime time = new DateTime();
            product.name = deviceNameTextBox.Text;
            product.type = "client";
            product.bootup_time = "2012-12-04 23:17:42";

            string output = JsonConvert.SerializeObject(product);

        
            String connectionString;
            // Make sure we can perform this action with valid data
            if (ValidateRemoteHost() && ValidateInput())
            {
                // Instantiate the SocketClient
              

                // Attempt to connect to the echo server
                string result = client.Connect(ServerAddressTextBox.Text, ECHO_PORT);
                connectionStatusTextBLock.Text = "Connection Status: Online";

                result = client.Send(output);
                Log(result, false);
                result = client.Send("list");
                result = client.Receive();
                //Log(result, false); 
                DeviceList devices = JsonConvert.DeserializeObject<DeviceList>(result);
                Log(devices.deviceListFromServer[0] , false);
                while (devices.deviceListFromServer == null)
                {
                }
                 DeviceItems = new ObservableCollection<ItemViewModel>();
                 for (int i = 0; i < devices.deviceListFromServer.Length; i++)
                 {
                     DeviceItems.Add(new ItemViewModel() { LineOne = devices.deviceListFromServer[i], LineTwo = "view devices linked to this phone", LineThree = "" });
                    
                 }

                 myDeviceListBox.ItemsSource = DeviceItems;


                // Receive a response from the echo server
           
                result = client.Receive();
               
            }
        }

        private void allDevicesListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            String deviceId= "";
            if(null != allDevicesListBox.SelectedItem)
            {
                deviceId = (allDevicesListBox.SelectedItem as ItemViewModel).LineOne;
            }
            client.Send("connect " + deviceId);
            connectedDevices = new ObservableCollection<ItemViewModel>();
            connectedDevices.Add(new ItemViewModel() { LineOne = deviceId, LineTwo = "connected device", LineThree = "" });
            myDeviceListBox.ItemsSource = connectedDevices;
            client.Send("relay hopefully this is working");
        //    connectedDevices.Add(new ItemViewModel() { LineOne = allDevicesListBox.SelectedValue.ToString(), LineTwo = "Connected Remote Device", LineThree = "" });
        }
        

        
    }
}