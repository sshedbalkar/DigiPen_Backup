using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Xml.Serialization;
using Forms = System.Windows.Forms;
using System.ComponentModel;
using System.Text;
using System.Drawing.Design;
using System.Windows.Forms;
using CustomUITypeEditors;
using Microsoft.Xna.Framework;

namespace GLEED2D
{
    public partial class root
    {
        [XmlIgnore()]
        public string selectedlayers;
        [XmlIgnore()]
        public string selecteditems;

        public class EditorVars
        {
            public int NextItemNumber;
            public string ContentrootFolder;
        }

        [XmlIgnore()]
        [Category(" General")]
        [Description("When the root is saved, each texture is saved with a path relative to this folder."
                     + "You should set this to the \"Content.rootDirectory\" of your game project.")]
        [EditorAttribute(typeof(FolderUITypeEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public String ContentrootFolder
        {
            get
            {
                return EditorRelated.ContentrootFolder;
            }
            set
            {
                EditorRelated.ContentrootFolder = value;
            }
        }

        EditorVars editorrelated = new EditorVars();
        [Browsable(false)]
        public EditorVars EditorRelated
        {
            get
            {
                return editorrelated;
            }
            set
            {
                editorrelated = value;
            }
        }

        [XmlIgnore()]
        public Forms.TreeNode treenode;


        public int getNextItemNumber()
        {
            return (++EditorRelated.NextItemNumber);//.ToString("00000");
        }


        public void export(string filename)
        {
            foreach (Layer l in Level.Layers)
            {
                foreach (Item i in l.Items)
                {
                    if (i is TextureItem)
                    {
                        TextureItem ti = (TextureItem)i;
                        ti.texture_filename = RelativePath(ContentrootFolder, ti.texture_fullpath);
                        ti.asset_name = ti.texture_filename.Substring(0, ti.texture_filename.LastIndexOf('.'));
                    }
                }
            }



            XmlTextWriter writer = new XmlTextWriter(filename, null);
            writer.Formatting = Formatting.Indented;
            writer.Indentation = 4;

            XmlSerializerNamespaces ns = new XmlSerializerNamespaces();
            ns.Add("", "");
            XmlSerializer serializer = new XmlSerializer(typeof(root));
            serializer.Serialize(writer, this, ns);

            writer.Close();

        }



        public string RelativePath(string relativeTo, string pathToTranslate)
        {
            string[] absoluteDirectories = relativeTo.Split('\\');
            string[] relativeDirectories = pathToTranslate.Split('\\');

            //Get the shortest of the two paths
            int length = absoluteDirectories.Length < relativeDirectories.Length ? absoluteDirectories.Length : relativeDirectories.Length;

            //Use to determine where in the loop we exited
            int lastCommonroot = -1;
            int index;

            //Find common root
            for (index = 0; index < length; index++)
                if (absoluteDirectories[index] == relativeDirectories[index])
                    lastCommonroot = index;
                else
                    break;

            //If we didn't find a common prefix then throw
            if (lastCommonroot == -1)
                // throw new ArgumentException("Paths do not have a common base");
                return pathToTranslate;

            //Build up the relative path
            StringBuilder relativePath = new StringBuilder();

            //Add on the ..
            for (index = lastCommonroot + 1; index < absoluteDirectories.Length; index++)
                if (absoluteDirectories[index].Length > 0) relativePath.Append("..\\");

            //Add on the folders
            for (index = lastCommonroot + 1; index < relativeDirectories.Length - 1; index++)
                relativePath.Append(relativeDirectories[index] + "\\");

            relativePath.Append(relativeDirectories[relativeDirectories.Length - 1]);

            return relativePath.ToString();
        }


     }


    public partial class Level
    {

        [XmlIgnore]
        public root root;

        public Level(String name)
            : this()
        {
            this.Name = name;
            this.Visible = true;
        }

        public Level clone()
        {
            Level result = (Level)this.MemberwiseClone();
            result.Layers = new List<Layer>(Layers);
            for (int i = 0; i < result.Layers.Count; i++)
            {
                result.Layers[i] = result.Layers[i].clone();
                result.Layers[i].level = result;
            }
            return result;
        }



        //public Item getItemAtPos(Vector2 mouseworldpos)
        //{
        //    for (int i = Items.Count - 1; i >= 0; i--)
        //    {
        //        if (Items[i].contains(mouseworldpos) && Items[i].Visible) return Items[i];
        //    }
        //    return null;
        //}

        //public void drawInEditor(SpriteBatch sb)
        //{
        //    if (!Visible) return;
        //    foreach (Item item in Items) item.drawInEditor(sb);
        //}



    }






}
