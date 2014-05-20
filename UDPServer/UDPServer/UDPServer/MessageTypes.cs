using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UDPServer
{
    class MessageTypes
    {
        public enum Server
        {
            AddExistingUsers, MovementUpdate, AddNewUser,ReadyUpdate, StartGame
        }

        public enum Client
        {
            Login, MovementUpdate,ReadyUpdate
        }
    }
}
