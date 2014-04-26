using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp
{
    public class MessageTypes
    {
        
        public enum Server
        {
            AddExistingUsers,MovementUpdate, AddNewUser
        }

        public enum Client
        {
            Login, MovementUpdate
        }
        
    }
}
