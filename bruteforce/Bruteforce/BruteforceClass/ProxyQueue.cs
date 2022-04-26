using Leaf.xNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GDH.Special.Bruteforce
{
    public static class ProxyQueue
    {
        public static void UpdateList(List<string> list)
        {
            Proxies = list;
            currentProxy = 0;
        }

        public static List<string> Proxies { get; private set; }
        private static int currentProxy = 0;

        public static ProxyClient NextProxy()
        {
            if (Proxies.Count == 0)
                return null;
            ProxyClient client = null;
            try
            {
                string[] parts = Proxies[currentProxy].Split(':');
                client = new Socks4ProxyClient(parts[0], int.Parse(parts[1]));
            }
            catch { }
            currentProxy++; currentProxy %= Proxies.Count;
            return client;
        }
    }
}
