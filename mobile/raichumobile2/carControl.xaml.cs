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
using Microsoft.Devices.Sensors;
using Newtonsoft.Json;
namespace raichumobile2
{
    public partial class carControl : PhoneApplicationPage
    {
        public partial class Vector3
        {
            public float X;
            public float Y;
            public float Z;
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

        String SERVER_IP = "130.65.157.121";
        int SERVER_PORT = 30000;
        Accelerometer accelSensor;
        Vector3 accelReading = new Vector3();
        String deviceID;
        SocketClient client = new SocketClient();
        public carControl()
        {
            InitializeComponent();
            accelSensor = new Accelerometer();
            accelSensor.ReadingChanged +=new EventHandler<AccelerometerReadingEventArgs>(AccelerometerReadingChanged);
        }
        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {

            this.deviceID = this.NavigationContext.QueryString["deviceId"];
            serverConnect();


        }
        public void serverConnect()
        {
            // Clear the log 
            Device product = new Device();
            DateTime time = new DateTime();
            product.name = "mp3Controller";
            product.type = "client";
            product.bootup_time = "2012-12-04 23:17:42";

            string output = JsonConvert.SerializeObject(product);


            client.Connect(SERVER_IP, SERVER_PORT);
            //client.Connect(SERVER_IP, SERVER_PORT);
            client.Send(output);
            client.Send("connect " + deviceID);

            client.Receive();

        }
        public void AccelerometerReadingChanged(object sender, AccelerometerReadingEventArgs e)
        {
            accelReading.X = (float)e.X;
            accelReading.Y = (float)e.Y;
            accelReading.Z = (float)e.Z;
            speedValue.Text = accelReading.X.ToString();
            turnValue.Text = accelReading.Y.ToString();

        }
        public void startAccel()
        {
            bool accelActive;
            accelSensor = new Accelerometer();
           // accelSensor.CurrentValueChanged += new EventHandler<AccelerometerReadingEventArgs>(AccelerometerReadingChanged); 
            try
            {
                accelSensor.Start();
                accelActive = true;
            }
            catch (AccelerometerFailedException e)
            {
                // the accelerometer couldn't be started.  No fun!
                accelActive = false;
            }
            catch (UnauthorizedAccessException e)
            {
                // This exception is thrown in the emulator-which doesn't support an accelerometer.
                accelActive = false;
            }
        }


    }
}