using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace CustomCapability
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private RpcClientUWP.RpcClient1 m_rpcClient;
        private bool m_rpcRunning;
        private Audio.AudioOutput m_audioOutput;

        public MainPage()
        {
            this.InitializeComponent();
        }

        public void NavigateToPageWithParameter(object parameter)
        {

        }

        private void OnAudioInputHandler(IBuffer buffer)
        {
            //Debug.WriteLine("OnAudioInputHandler: " + buffer.Length);
            m_audioOutput.Send(buffer);
        }

        private async void OpenButton_Click(object sender, RoutedEventArgs e)
        {
            if(m_rpcClient == null)
            {
                m_rpcClient = new RpcClientUWP.RpcClient1();
                m_rpcClient.OnAudioInput += OnAudioInputHandler;
                var result = await m_rpcClient.Open();
                if(result)
                {
                    openButton.Content = "Close";
                    startButton.IsEnabled = true;
                    m_rpcRunning = false;
                }
                else
                {
                    m_rpcClient = null;
                }
            }
            else
            {
                var result = await m_rpcClient.Close();
                openButton.Content = "Open";
                startButton.IsEnabled = false;
                m_rpcClient = null;
            }
        }

        private async void StartButton_Click(object sender, RoutedEventArgs e)
        {
            if (m_rpcRunning)
            {
                var result = await m_rpcClient.Stop();
                m_rpcRunning = false;
                startButton.Content = "Start";
                openButton.IsEnabled = true;
                m_audioOutput.Stop();
                m_audioOutput = null;
            }
            else
            {
                m_audioOutput = new Audio.AudioOutput();
                await m_audioOutput.Start();
                m_rpcClient.Start();
                m_rpcRunning = true;
                startButton.Content = "Stop";
                openButton.IsEnabled = false;
            }
        }
    }
}
