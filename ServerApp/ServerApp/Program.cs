using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
namespace ServerApp
{
    class Program
    {
        static void Main(string[] args)
        {
            AsynchronousSocketListener.StartListening();
        }

        public class StateObject
        {
            // Client  socket.
            public Socket workSocket = null;
            // Size of receive buffer.
            public const int BufferSize = 512;
            // Receive buffer.
            public byte[] buffer = new byte[BufferSize];
            // Received data string.
            public StringBuilder sb = new StringBuilder();
        }

        public class AsynchronousSocketListener
        {
            // Thread signal.
            public static ManualResetEvent allDone = new ManualResetEvent(false);
            public static Dictionary<Socket, int> userList = new Dictionary<Socket,int>();
            public static Dictionary<Socket, string> sentValues = new Dictionary<Socket, string>();
            public static int userCounter = 1;

            public AsynchronousSocketListener()
            {

            }

            public static void StartListening()
            {
                // Data buffer for incoming data.
                byte[] bytes = new Byte[512];

                // Establish the local endpoint for the socket.
                // The DNS name of the computer
                // running the listener is "host.contoso.com".
                //143.24.20.36
                //24-14-18-8f
                //01-00-00-7f
                
                //100.66.70.6
                //06-46-42-64
                //IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Parse("100.66.70.6"), 8080);
                IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 8080);
                // Create a TCP/IP socket.
                Socket listener = new Socket(AddressFamily.InterNetwork,
                    SocketType.Stream, ProtocolType.Tcp);

                // Bind the socket to the local endpoint and listen for incoming connections.
                try
                {
                    listener.Bind(localEndPoint);
                    listener.Listen(100);

                    while (true)
                    {
                        // Set the event to nonsignaled state.
                        allDone.Reset();

                        // Start an asynchronous socket to listen for connections.
                        Console.WriteLine("Waiting for a connection...");
                        listener.BeginAccept(
                            new AsyncCallback(AcceptCallback),
                            listener);

                        // Wait until a connection is made before continuing.
                        allDone.WaitOne();
                    }

                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }

                Console.WriteLine("\nPress ENTER to continue...");
                Console.Read();

            }

            public static void AcceptCallback(IAsyncResult ar)
            {
                try
                {// Signal the main thread to continue.
                    allDone.Set();

                    // Get the socket that handles the client request.
                    Socket listener = (Socket)ar.AsyncState;
                    Socket handler = listener.EndAccept(ar);

                    // Create the state object.
                    StateObject state = new StateObject();
                    state.workSocket = handler;
                    userList.Add(handler, userCounter);
                    sentValues.Add(handler, "0,0,0,1,0,0,0,1,0,0,0,1");
                    userCounter++;
                    handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                        new AsyncCallback(ReadCallback), state);
                }
                catch (SocketException e)
                {
                    Console.WriteLine(e.Message);
                }
            }

            public static void ReadCallback(IAsyncResult ar)
            {
                String content = String.Empty;

                // Retrieve the state object and the handler socket
                // from the asynchronous state object.
                StateObject state = (StateObject)ar.AsyncState;
                Socket handler = state.workSocket;
                int bytesRead = 0;
                try
                {
                    // Read data from the client socket.
                    
                    bytesRead = handler.EndReceive(ar);
                }
                catch (SocketException e)
                {
                    userList.Remove(handler);
                    sentValues.Remove(handler);
                    Console.WriteLine(e.Message);
                }
                if (bytesRead > 0)
                {
                    // There  might be more data, so store the data received so far.
                    state.sb.Append(Encoding.ASCII.GetString(
                        state.buffer, 0, bytesRead));
                    // Check for end-of-file tag. If it is not there, read 
                    // more data.
                    content = state.sb.ToString();
                    if (content.IndexOf("<EOF>") > -1)
                    {
                        content = content.Substring(0, content.IndexOf("<EOF>"));
                        // All the data has been read from the 
                        // client. Display it on the console.
                        Console.WriteLine("Read {0} bytes from socket. \nData : {1}",
                            content.Length, content);

                        string[] splitContent = content.Split('\n');
                        MessageTypes.Client messageType = (MessageTypes.Client)int.Parse(splitContent[0]);

                        if (messageType.Equals(MessageTypes.Client.Login))
                        {
                            string toAlreadyAdded = userList[handler].ToString();
                            string sendToAdded = userList[handler].ToString() + "\n";

                            foreach (KeyValuePair<Socket, string> entry in sentValues)
                            {
                                if(!entry.Key.Equals(handler))
                                sendToAdded += userList[entry.Key] + "\n" + entry.Value + "\n";
                            }

                            foreach (KeyValuePair<Socket, int> entry in userList)
                            {
                                if (entry.Key.Equals(handler))
                                {
                                    Send(entry.Key, MessageTypes.Server.AddExistingUsers, sendToAdded);
                                }
                                else
                                {
                                    Send(entry.Key, MessageTypes.Server.AddNewUser, toAlreadyAdded);
                                }
                            }
                        }
                        else if (messageType.Equals(MessageTypes.Client.MovementUpdate))
                        {
                            sentValues[handler] = splitContent[1];

                            string toSend = "";
                            foreach (KeyValuePair<Socket, int> entry in userList)
                            {
                                toSend += entry.Value + "\n" + sentValues[entry.Key] + "\n";
                            }
                            SendAll(MessageTypes.Server.MovementUpdate,toSend);
                        }
                    }
                    else
                    {
                        // Not all data received. Get more.
                        handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                        new AsyncCallback(ReadCallback), state);
                    }
                }
            }

            private static void Send(Socket handler, MessageTypes.Server type, String data = "")
            {
                // Convert the string data to byte data using ASCII encoding.
                byte[] byteData = Encoding.ASCII.GetBytes((int)type + "\n" + data);
                // Begin sending the data to the remote device.
                handler.BeginSend(byteData, 0, byteData.Length, 0,
                    new AsyncCallback(SendCallback), handler);
            }

            private static void Send(Socket handler, MessageTypes.Server type, int data)
            {
                Send(handler, type, "" + data);
            }

            private static void SendAll(MessageTypes.Server type, String data)
            {
                byte[] byteData = Encoding.ASCII.GetBytes((int)type + "\n" + data);
                foreach (KeyValuePair<Socket, int> entry in userList)
                {
                    entry.Key.BeginSend(byteData, 0, byteData.Length, 0,
                    new AsyncCallback(SendCallback), entry.Key);
                }
            }

            private static void SendCallback(IAsyncResult ar)
            {
                Socket handler = (Socket)ar.AsyncState;
                try{

                    // Complete sending the data to the remote device.
                    int bytesSent = handler.EndSend(ar);
                    Console.WriteLine("Sent {0} bytes to client.", bytesSent);
                    StateObject state = new StateObject();
                    state.workSocket = handler;
                    handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                    new AsyncCallback(ReadCallback), state);
                    //handler.Shutdown(SocketShutdown.Both);
                    //handler.Close();

                }
                catch (Exception e)
                {
                    userList.Remove(handler);
                    sentValues.Remove(handler);
                    Console.WriteLine(e.ToString());
                }
            }



        }
    }
}