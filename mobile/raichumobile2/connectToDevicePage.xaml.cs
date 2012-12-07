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

namespace raichumobile2.ViewModels
{
    
    public partial class connectToDevicePage : PhoneApplicationPage
    {


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
        String deviceID;
        String Status;
        SocketClient client = new SocketClient();
        String SERVER_IP = "130.65.157.121";
        int SERVER_PORT = 30000;

        public connectToDevicePage()
        {
            InitializeComponent();
            
        }  
             protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
            {
                 
                this.deviceID = this.NavigationContext.QueryString["deviceId"];
                serverConnect();


            }
             public  void serverConnect()
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
                client.Send("connect " +deviceID );
             
                client.Receive();
              
            }

            private void playButton_Tap(object sender, GestureEventArgs e)
            {
                client.Send("relay play");
                Status = client.Receive();
                mp3Status.Text = Status;

            }

            private void pauseButton_Tap(object sender, GestureEventArgs e)
            {
                client.Send("relay pause");
                Status = client.Receive();
                mp3Status.Text = Status;
            }

            private void stopButton_Tap(object sender, GestureEventArgs e)
            {
                client.Send("relay stop");
                Status = client.Receive();
                mp3Status.Text = Status;
            }

            private void disconnectButton_Tap(object sender, GestureEventArgs e)
            {
                client.Close();
                //client.Receive();
                this.NavigationService.Navigate(new Uri("/MainPage.xaml", UriKind.Relative));
            }

            private void volDownButton_Tap(object sender, GestureEventArgs e)
            {
                client.Send("relay volDown");
                Status = client.Receive();
                mp3Status.Text = Status;
            }

            private void volUpButton_Tap(object sender, GestureEventArgs e)
            {
                client.Send("relay volUp");
                Status = client.Receive();
                mp3Status.Text = Status;
            }

            private void nextButton_Tap(object sender, GestureEventArgs e)
            {
                client.Send("relay forward");
                Status = client.Receive();
                mp3Status.Text = Status;
            }

            private void prevButton_ImageFailed(object sender, ExceptionRoutedEventArgs e)
            {

            }

            private void prevButton_Tap(object sender, GestureEventArgs e)
            {
                client.Send("relay back");
                Status = client.Receive();
                mp3Status.Text = Status;
            }

            
       }
}

