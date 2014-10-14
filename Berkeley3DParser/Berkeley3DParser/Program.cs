using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;

namespace Berkeley3DParser
{
    class Program
    {
        /// <summary>
        /// convert xml labels to simple txt for c++ to read
        /// </summary>
        /// <param name="args">The arguments.</param>
        static void Main(string[] args)
        {
            string b3d_dir = @"E:\Datasets\RGBD_Dataset\Berkeley\VOCB3DO\Annotations\";

            string[] files = Directory.GetFiles(b3d_dir, "*.xml");

            foreach (string fn in files)
            {
                StringBuilder xml_content = new StringBuilder();

                XmlDocument xml_reader = new XmlDocument();
                xml_reader.Load(fn);
                int obj_num = 0;
                foreach (XmlNode node in xml_reader.ChildNodes[0].ChildNodes)
                {
                    if (node.Name == "object")
                    {
                        obj_num++;
                        foreach (XmlNode objnode in node.ChildNodes)
                        {
                            switch (objnode.Name)
                            {
                                case "name":
                                    xml_content.AppendLine(objnode.InnerText);
                                    break;
                                case "bndbox":
                                    xml_content.AppendLine(objnode.ChildNodes[1].InnerText);
                                    xml_content.AppendLine(objnode.ChildNodes[3].InnerText);
                                    xml_content.AppendLine(objnode.ChildNodes[0].InnerText);
                                    xml_content.AppendLine(objnode.ChildNodes[2].InnerText);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }

                xml_content.Insert(0, obj_num.ToString() + "\n");
                string savefn = Path.GetDirectoryName(fn) + "\\" + Path.GetFileNameWithoutExtension(fn) + ".txt";
                using (StreamWriter writer = new StreamWriter(savefn))
                {
                    writer.Write(xml_content.ToString());
                }

                Console.WriteLine("Finished file " + fn);

            }
           
            
        }
    }
}
