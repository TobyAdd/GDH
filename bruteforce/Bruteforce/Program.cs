using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using GDH.Special.Bruteforce;

namespace Bruteforce
{
    static class ExtensionsClass
    {
        private static Random rng = new Random();

        public static void Shuffle<T>(this IList<T> list)
        {
            int n = list.Count;
            while (n > 1)
            {
                n--;
                int k = rng.Next(n + 1);
                T value = list[k];
                list[k] = list[n];
                list[n] = value;
            }
        }
    }

    class Program
    {
        //Generator
        static string charset = "";
        static int startlenght = 6;
        static int limitlenght = 8;

        //Wordlist
        static string wordlist = "";

        //Target
        static string username = "";

        //Proxy
        static List<string> proxylist = new List<string>();
        static string proxylistpath = "";

        //Random
        static Random rng = new Random();

        static Thread attackThread = null;
        private static void TerminateThread()
        {
            if (attackThread != null)
            {
                if (attackThread.IsAlive)
                    attackThread.Abort();
                attackThread = null;
            }
        }

        static void LoadProxyList()
        {
            List<string> ls = new List<string>();
            foreach (var proxy in proxylist)
                ls.Add(proxy.ToString());
            ProxyQueue.UpdateList(ls);
        }

        static void Main(string[] args)
        {

            Console.Title = "Bruteforce - GDH\n";
            Console.Write("Bruteforce by TobyAdd\n");
            Console.Write("Original bruteforce by Eimaen");
            
            Console.ReadKey();
            Console.Clear();

            while (true)
            {
                Console.Write("--Generator--\n");
                Console.Write("1.1) Character set: "); if (charset == "") { Console.Write("No characters\n"); } else { Console.Write(charset + "\n"); }
                Console.Write("1.2) Start length: " + startlenght.ToString() + "\n");
                Console.Write("1.3) Limit length: " + limitlenght.ToString() + "\n");
                Console.Write("1.4) What?\n");
                Console.Write("--Wordlist--\n");
                Console.Write("2.1) Wordlist path: "); if (wordlist == "") { Console.Write("No path\n"); } else { Console.Write(wordlist + "\n"); }
                Console.Write("2.2) What?\n");
                Console.Write("--Target--\n");
                Console.Write("3.1) Target username: "); if (username == "") { Console.Write("No username\n"); } else { Console.Write(username + "\n"); }
                Console.Write("--Proxy Management--\n");
                Console.Write("4.1) Add proxy URL\n");
                Console.Write("Proxy list: "); if (!proxylist.Any()) { Console.Write("No proxy yet\n"); } else { Console.Write("\n"); proxylist.ForEach(Console.WriteLine); }
                Console.Write("4.2) Clear All Proxy\n");
                Console.Write("4.3) Shuffle\n");
                Console.Write("--Proxy import--\n");
                Console.Write("5.1) Proxy list path: "); if (proxylistpath == "") { Console.Write("No path\n"); } else { Console.Write(proxylistpath + "\n"); }
                Console.Write("5.2) Load\n");
                Console.Write("5.3) What?\n");
                Console.Write("--Actions--\n");
                Console.Write("6.1) Dictionary Attack\n");
                Console.Write("6.2) Generator Attack\n\n");
                Console.Write("Bruteforce> ");
                var command = Console.ReadLine();

                //Generator
                if (command == "1.1")
                {
                    Console.Write("Character set: ");
                    charset = Console.ReadLine();
                }
                else if (command == "1.2")
                {
                    Console.Write("Start length(Numbers Only): ");
                    try { startlenght = Convert.ToInt32(Console.ReadLine()); } catch { Console.Write("Error!"); Console.ReadKey(); }
                }
                else if (command == "1.3")
                {
                    Console.Write("Limit length(Numbers Only): ");
                    try { limitlenght = Convert.ToInt32(Console.ReadLine()); } catch { Console.Write("Error!"); Console.ReadKey(); }
                }
                else if (command == "1.4")
                {
                    Console.Write("Help: " + "Enter in the field the letters that will be in the password, also specify the starting and ending length. " +
            "A wordlist will be generated from all permutations of letters in the sequence.\n" +
            "Tip: Do not repeat letters twice, this will complicate the algorithm.");
                    Console.ReadKey();
                }
                //Generator

                //Wordlist
                else if (command == "2.1")
                {
                    Console.Write("Wordlist(Please, write avaible path to txt file): ");
                    wordlist = Console.ReadLine();

                    if (!System.IO.File.Exists(wordlist)) { wordlist = ""; Console.Write("Not avaible txt file"); Console.ReadKey(); }
                }
                else if (command == "2.1")
                {
                    Console.Write("Wordlist(Please, write avaible path to txt file): ");
                    wordlist = Console.ReadLine();

                    if (!System.IO.File.Exists(wordlist)) { wordlist = ""; Console.Write("Not avaible txt file"); Console.ReadKey(); }
                }
                else if (command == "2.2")
                {
                    Console.Write("Help: " + "Enter the path to the list of words to check.\n" +
            "Format: one password per line.");
                    Console.ReadKey();
                }
                //Wordlist

                //Target
                else if (command == "3.1")
                {
                    Console.Write("Target username: ");
                    username = Console.ReadLine();
                }
                //Target

                //Proxy Management
                else if (command == "4.1")
                {
                    Console.Write("Enter a proxy: ");
                    proxylist.Add(Console.ReadLine());
                }
                else if (command == "4.2")
                {
                    proxylist.Clear();
                }
                else if (command == "4.3")
                {
                    proxylist.Shuffle();
                }

                //Proxy Management

                //Proxy Import
                else if (command == "5.1")
                {
                    Console.Write("Proxy list:(Please, write avaible path to txt file): ");
                    proxylistpath = Console.ReadLine();

                    if (!System.IO.File.Exists(wordlist)) { proxylistpath = ""; Console.Write("Not avaible txt file"); Console.ReadKey(); }
                }

                else if (command == "6.1")
                {
                    TerminateThread();
                    LoadProxyList();
                    attackThread = new Thread(() =>
                    {
                        var result = BruteforceGDH.DictionaryAttack(username, File.ReadAllLines(wordlist));
                        if (result == string.Empty){
                            Console.Write("\nNone of the passwords on this list matched.");
                        }
                        else {
                            Console.Write($"\nA password for {username} is \"{result}\"!");
                        }                            
                    });
                    attackThread.Start();
                }
                //Proxy Import

                //Actions
                else if (command == "6.2")
                {
                    TerminateThread();
                    LoadProxyList();
                    attackThread = new Thread(() =>
                    {
                        var result = BruteforceGDH.GeneratorAttack(username, charset.ToArray(), (int)startlenght, (int)limitlenght);
                        if (result == string.Empty){
                            Console.Write("\nNone of the generated passwords matched.");
                            Console.ReadKey();
                        }
                        else{
                            Console.Write($"A password for {username} is \"{result}\"!");
                            Console.ReadKey();
                        }                            
                    });
                    attackThread.Start();
                }
                //Actions
                Console.Clear();
            }
        }
    }
}
