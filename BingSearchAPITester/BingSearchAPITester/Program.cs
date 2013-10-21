using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.Services.Client;
using System.Net;
using System.IO;

namespace BingSearchAPITester
{
    class Program
    {
        private const string AccountKey = "z0+iOTHqhv3yMYEFP072ZYHXNlQLqGm2LlLCvCBdGE4=";
        private const string SaveDir = "H:/Dataset/BingData/";

        static void Main(string[] args)
        {
            try
            {
                MakeRequest();
                Console.ReadLine();
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
                if (ex.InnerException != null)
                    Console.WriteLine(ex.InnerException.Message);
            }
        }

        static void MakeRequest()
        {
            string query = "Chairs";
            string res_img_dir = SaveDir + query + "/";
            // create directory
            Directory.CreateDirectory(res_img_dir);

            string rootUrl = "https://api.datamarket.azure.com/Bing/Search";
            var bingContainer = new Bing.BingSearchContainer(new Uri(rootUrl));

            string market = "en-us";

            bingContainer.Credentials = new NetworkCredential(AccountKey, AccountKey);

            // build query
            var imageQuery = bingContainer.Image(query, null, market, null, null, null, null);
            imageQuery = imageQuery.AddQueryOption("$top", 50);
            imageQuery = imageQuery.AddQueryOption("$skip", 50);

            // run query: MUST ADD TOLIST()!
            var imageResults = imageQuery.Execute().ToList();

            Console.WriteLine("{0} results for query " + query + ":", imageResults.Count());

            // download searched images
            System.Net.WebClient _WebClient = new System.Net.WebClient();
            int cnt = 0;
            foreach(var result in imageResults)
            {
                cnt++;
                Console.WriteLine("{0}: {1}\n{2}\n", cnt, result.Title, result.MediaUrl);
                int filenamestart = result.MediaUrl.LastIndexOf('/') + 1;
                int filenameend = result.MediaUrl.LastIndexOf('.') - 1;
                string filename = result.MediaUrl.Substring(filenamestart, filenameend - filenamestart + 1);
                _WebClient.DownloadFile(result.MediaUrl, res_img_dir + filename + ".jpg");
            }
        }
    }
}
