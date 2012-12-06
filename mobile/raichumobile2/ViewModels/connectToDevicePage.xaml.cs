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

namespace raichumobile2.ViewModels
{
    public partial class connectToDevicePage : PhoneApplicationPage
    {
        String deviceID;
        public connectToDevicePage()
        {
            InitializeComponent();


            
        }
        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            this.deviceID = this.NavigationContext.QueryString["deviceId"];
            deviceIdTextBlock.Text = this.deviceID;
            base.OnNavigatedTo(e);
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            
            this.NavigationService.Navigate(new Uri("/MainPage.xaml?connect=1?deviceId="+this.deviceID, UriKind.Relative));

        }
    }
}