using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Timers;
namespace UDPServer
{
    class Program
    {
        static void Main(string[] args)
        {
            AsynchronousSocketListener a = new AsynchronousSocketListener();
        }

        public class StateObject
        {
            // Client  socket.
            public IPEndPoint endpoint = null;
            // Size of receive buffer.
            public string whatWasSent = null;

            public StateObject(IPEndPoint ws, string wss)
            {
                endpoint = ws;
                whatWasSent = wss;
            }
        }
        public class AsynchronousSocketListener
        {
            // Thread signal.
            public static List<string> playerNames = new List<string>();
            public static Dictionary<IPEndPoint, int> userList = new Dictionary<IPEndPoint, int>();
            public static Dictionary<IPEndPoint, string> sentValues = new Dictionary<IPEndPoint, string>();
            public static Dictionary<int, string> nameMap = new Dictionary<int, string>();
            public static Dictionary<int, bool> readyUp = new Dictionary<int, bool>();
            public static int userCounter = 0;
            static IPEndPoint ipep;
            static UdpClient newsock;
            public static int msgCount= 0;

            public AsynchronousSocketListener()
            {
                playerNames.Add("Sam");
                playerNames.Add("Nick");
                playerNames.Add("Rushabh");
                playerNames.Add("Arun");
                playerNames.Add("Alex");
                ipep = new IPEndPoint(IPAddress.Any, 8080); 
                newsock = new UdpClient(ipep);

                Console.WriteLine("Waiting for a client...");
                try
                {
                    //starts the initial listen for a received connection
                    newsock.BeginReceive(new AsyncCallback(recv), null);
                    Timer aTimer = new System.Timers.Timer(100);

                    // Hook up the Elapsed event for the timer.
                    aTimer.Elapsed += new ElapsedEventHandler(OnMovementUpdateEvent);

                    aTimer.Interval = 100;
                    aTimer.Enabled = true;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }
                Console.Read();
            }

            private static void recv(IAsyncResult res)
            {

                //grabs the data sent from the client
                IPEndPoint endpoint = new IPEndPoint(IPAddress.Any, 8000);
                try
                {
                    byte[] received = newsock.EndReceive(res, ref endpoint);
                    string[] splitContent = Encoding.ASCII.GetString(received).Split('\n');
                    if (splitContent != null)
                    {
                        MessageTypes.Client messageType = (MessageTypes.Client)int.Parse(splitContent[0]);
                        switch (messageType)
                        {
                            case MessageTypes.Client.Login:
                                if (!userList.ContainsKey(endpoint))
                                {
                                    //adds the new client data to the dictionaries
                                    userCounter++;

                                    string newPlayerName = playerNames[0];
                                    playerNames.RemoveAt(0);
                                    nameMap.Add(userCounter, newPlayerName);
                                    userList.Add(endpoint, userCounter);
                                    sentValues.Add(endpoint, "0,0,0,0,0,0,1,0,0,0,1,0,0,0,1");
                                    readyUp.Add(userCounter, false);

                                    //to the players already in the game, just send the id of the new player
                                    string toAlreadyAdded = userList[endpoint].ToString() + "\n" + newPlayerName;

                                    //to the player currently entering the game, send your ID first
                                    string sendToAdded = userList[endpoint].ToString() + "\n" + newPlayerName + "\n";

                                    foreach (KeyValuePair<IPEndPoint, string> entry in sentValues)
                                    {
                                        if (!entry.Key.Equals(endpoint))
                                            sendToAdded += userList[entry.Key] + "\n" + nameMap[userList[entry.Key]] + "\n" + entry.Value + "\n";
                                    }

                                    foreach (KeyValuePair<IPEndPoint, int> entry in userList)
                                    {
                                        if (entry.Key.Equals(endpoint))
                                        {
                                            Send(entry.Key, MessageTypes.Server.AddExistingUsers, sendToAdded);
                                        }
                                        else
                                        {
                                            Send(entry.Key, MessageTypes.Server.AddNewUser, toAlreadyAdded);
                                        }
                                    }
                                }
                                break;

                            case MessageTypes.Client.MovementUpdate:
                                sentValues[endpoint] = splitContent[1];
                                break;

                            case MessageTypes.Client.ReadyUpdate:
                                if(int.Parse(splitContent[2])== 1){
                                    readyUp[userList[endpoint]]= true;
                                }else{
                                    readyUp[userList[endpoint]]= false;
                                }

                                bool canStart = true;
                                
                                foreach (KeyValuePair<int, bool> entry in readyUp)
                                {
                                    if (!entry.Value)
                                    {
                                        canStart = false;
                                    }

                                }

                                if (canStart)
                                {
                                    Console.WriteLine("GAME START");
                                    foreach (KeyValuePair<IPEndPoint, int> entry in userList)
                                    {
                                            Send(entry.Key, MessageTypes.Server.StartGame, "");
                                    }
                                }
                                else
                                {
                                    Console.WriteLine("Ready Up");
                                    foreach (KeyValuePair<IPEndPoint, int> entry in userList)
                                    {
                                        if (!entry.Key.Equals(endpoint))
                                        {
                                            Send(entry.Key, MessageTypes.Server.ReadyUpdate, splitContent[1] + "\n" + splitContent[2]);
                                        }
                                    }
                                }

                                break;

                        }
                        newsock.BeginReceive(new AsyncCallback(recv), null);
                    }
                }
                catch (Exception e)
                {
                    userList.Remove(endpoint);
                    sentValues.Remove(endpoint);
                    Console.WriteLine(e.ToString());
                }

                
            }

            private static void Send(IPEndPoint handler, MessageTypes.Server type, int data)
            {
                Send(handler, type, "" + data);
            }

            private static void Send(IPEndPoint handler, MessageTypes.Server type, String data = "")
            {
                msgCount++;
                // Convert the string data to byte data using ASCII encoding.
                string toSend=(int)type + "\n" + msgCount + "\n" + data;
                Byte[] byteData = Encoding.ASCII.GetBytes(toSend);
                newsock.BeginSend(byteData, byteData.Length, handler, new AsyncCallback(SendCallback), new StateObject(handler, toSend));
            }

            private static void SendAll(MessageTypes.Server type, String data)
            {
                byte[] byteData = Encoding.ASCII.GetBytes((int)type + "\n" + data);
                foreach (KeyValuePair<IPEndPoint, int> entry in userList)
                {
                    Send(entry.Key, type, data);
                }
            }

            private static void SendCallback(IAsyncResult ar)
            {
                StateObject sentTo = (StateObject)ar.AsyncState;
                try
                {
                    //Console.WriteLine("What was sent: " + sentTo.whatWasSent);
                }
                catch (Exception e)
                {
                    userList.Remove(sentTo.endpoint);
                    sentValues.Remove(sentTo.endpoint);
                    Console.WriteLine(e.ToString());
                }
            }


            private static void OnMovementUpdateEvent(object source, ElapsedEventArgs e)
            {
                //Console.WriteLine("The Elapsed event was raised at {0}", e.SignalTime);
                string toSend = "";
                foreach (KeyValuePair<IPEndPoint, int> entry in userList)
                {
                    toSend += entry.Value + "\n" + sentValues[entry.Key] + "\n";
                }
                SendAll(MessageTypes.Server.MovementUpdate, toSend);
            }

        }




    }
}
