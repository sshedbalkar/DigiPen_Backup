using System;
using System.Collections.Generic;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content;
using System.ComponentModel;
using System.IO;
using System.Xml;
using System.Xml.Serialization;


namespace GLEED2D1
{
    public partial class root
    {
        /// <summary>
        /// The name of the root.
        /// </summary>
        [XmlAttribute()]
        public String Name;

        [XmlIgnore()]
        [XmlAttribute()]
        public bool Visible;

        /// <summary>
        /// A root contains several Layers. Each Layer contains several Items.
        /// </summary>
        public List<Archetype> Archetypes;

        /// <summary>
        /// A Dictionary containing any user-defined Properties.
        /// </summary>
        /// 
        [XmlIgnore()]
        public SerializableDictionary CustomProperties;


        public root()
        {
            Visible = true;
            //level = new Level("Level");
            CustomProperties = new SerializableDictionary();
        }

        public static root FromFile(string filename, ContentManager cm)
        {
            FileStream stream = File.Open(filename, FileMode.Open);
            XmlSerializer serializer = new XmlSerializer(typeof(root));
            root root = (root)serializer.Deserialize(stream);
            stream.Close();
             foreach (Archetype arch in root.Archetypes)
             {
                 foreach (Component comp in arch.Components)
                {
                    //comp.load(cm);
                }
             }

            return root;
        }

        public static root FromFile(string filename)
        {
            FileStream stream = File.Open(filename, FileMode.Open);
            XmlSerializer serializer = new XmlSerializer(typeof(root));
            root root = (root)serializer.Deserialize(stream);
            stream.Close();

           

            return root;
        }


        public Component getItemByName(string name)
        {
            foreach (Archetype  arch in Archetypes )
            {
                foreach (Component comp in arch.Components )
                {
                    if (comp.Name == name) return comp;
                }
            }
            
            return null;
        }

        //public Layer getLayerByName(string name)
        //{
        //    foreach (Layer layer in this.level.Layers)
        //    {
        //        if (layer.Name == name) return layer;
        //    }
        //    return null;
        //}

        //public void draw(SpriteBatch sb)
        //{
        //   foreach (Layer layer in this.level.Layers) layer.draw(sb);
        //}


    }

    public partial class Archetype
    {
        /// <summary>
        /// The name of the level.
        /// </summary>
        [XmlAttribute()]
        public String Name;

        [XmlAttribute()]
        public String id;

        [XmlIgnore()]
        [XmlAttribute()]
        public bool Visible;

        /// <summary>
        /// A root contains several Layers. Each Layer contains several Items.
        /// </summary>
        public List<Component> Components;

        /// <summary>
        /// A Dictionary containing any user-defined Properties.
        /// </summary>
        /// 
        [XmlIgnore()]
        public SerializableDictionary CustomProperties;


        public Archetype()
        {
            Visible = true;
            Components = new List<Component>();
            CustomProperties = new SerializableDictionary();
        }


    }

     
    public partial class Component
    {
        /// <summary>
        /// The name of the layer.
        /// </summary>
        [XmlAttribute()]
        public String Name;

        /// <summary>
        /// Should this layer be visible?
        /// </summary>
        /// 
        //[XmlIgnore()]
        [XmlAttribute()]
        public bool Visible;

        /// <summary>
        /// The list of the items in this layer.
        /// </summary>
       // public List<Item> Items;

        /// <summary>
        /// The Scroll Speed relative to the main camera. The X and Y components are 
        /// interpreted as factors, so (1;1) means the same scrolling speed as the main camera.
        /// Enables parallax scrolling.
        /// </summary>
        /// 
        [XmlIgnore()]
        public Vector2 ScrollSpeed;

        /// <summary>
        /// A Dictionary containing any user-defined Properties.
        /// </summary>
        /// 
        [XmlIgnore()]
        public SerializableDictionary CustomProperties;


        public Component()
        {
            //Items = new List<Item>();
           // ScrollSpeed = Vector2.One;
            CustomProperties = new SerializableDictionary();
        }

       

    }

   
    public partial class CustomNode
    {
        /// <summary>
        /// The name of the layer.
        /// </summary>
        [XmlAttribute()]
        public String Type;

        //[XmlAttribute()]
        //public String Description;

        //[XmlAttribute()]
        //public Type Type;
       
        [XmlElement()]
        public String value;


        /// <summary>
        /// A Dictionary containing any user-defined Properties.
        /// </summary>
        // public SerializableDictionary CustomProperties;


        public CustomNode()
        {
            Type = "string";
            value = "";
        }

        public CustomNode(string s)
        {
            Type = s;
            value = "";
        }

        public void draw(SpriteBatch sb)
        {
            //if (!Visible) return;
            //foreach (Item item in Items) item.draw(sb);
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    //    NEEDED FOR SERIALIZATION. YOU SHOULDN'T CHANGE ANYTHING BELOW!
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////


    public class CustomProperty
    {
        public string name;
        public object value;
        public Type type;
        public string description;

        public CustomProperty()
        {
        }

        public CustomProperty(string n, object v, Type t, string d)
        {
            name = n;
            value = v;
            type = t;
            description = d;
        }

        public CustomProperty clone()
        {
            CustomProperty result = new CustomProperty(name, value, type, description);
            return result;
        }
    }


    public class SerializableDictionary : Dictionary<String, CustomProperty>, IXmlSerializable
    {

        public SerializableDictionary()
            : base()
        {

        }

        public SerializableDictionary(SerializableDictionary copyfrom)
            : base(copyfrom)
        {
            string[] keyscopy = new string[Keys.Count];
            Keys.CopyTo(keyscopy, 0);
            foreach (string key in keyscopy)
            {
                this[key] = this[key].clone();
            }
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {

            bool wasEmpty = reader.IsEmptyElement;
            reader.Read();

            if (wasEmpty) return;

            while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
            {
                CustomProperty cp = new CustomProperty();
                cp.name = reader.GetAttribute("Name");
                cp.description = reader.GetAttribute("Description");

                string type = reader.GetAttribute("Type");
               
                if (type == "string") cp.type = typeof(string);
                if (type == "bool") cp.type = typeof(bool);
                if (type == "Vector2") cp.type = typeof(Vector2);
                if (type == "Color") cp.type = typeof(Color);
                if (type == "Item") cp.type = typeof(Component);

                if (cp.type == typeof(Component))
                {
                    cp.value = reader.ReadInnerXml();
                    this.Add(cp.name, cp);
                }
                else
                {
                    reader.ReadStartElement("Property");
                    XmlSerializer valueSerializer = new XmlSerializer(cp.type);
                    object obj = valueSerializer.Deserialize(reader);
                    cp.value = Convert.ChangeType(obj, cp.type);
                    this.Add(cp.name, cp);
                    reader.ReadEndElement();
                }

                reader.MoveToContent();
            }
            reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            foreach (String key in this.Keys)
            {
                writer.WriteStartElement("Property");
                writer.WriteAttributeString("Name", this[key].name);
                if (this[key].type == typeof(string)) writer.WriteAttributeString("Type", "string");
                if (this[key].type == typeof(bool)) writer.WriteAttributeString("Type", "bool");
                if (this[key].type == typeof(Vector2)) writer.WriteAttributeString("Type", "Vector2");
                if (this[key].type == typeof(Color)) writer.WriteAttributeString("Type", "Color");
                if (this[key].type == typeof(Component)) writer.WriteAttributeString("Type", "Item");

                writer.WriteAttributeString("Description", this[key].description);

                if (this[key].type == typeof(Component))
                {
                    Component item = (Component)this[key].value;
                    if (item != null) writer.WriteString(item.Name);
                    else writer.WriteString("$null$");
                }
                else
                {
                    XmlSerializer valueSerializer = new XmlSerializer(this[key].type);
                    valueSerializer.Serialize(writer, this[key].value);
                }
                writer.WriteEndElement();
            }
        }

        /// <summary>
        /// Must be called after all Items have been deserialized. 
        /// Restores the Item references in CustomProperties of type Item.
        /// </summary>
        public void RestoreItemAssociations(root root)
        {
            foreach (CustomProperty cp in Values)
            {
                if (cp.type == typeof(Component)) cp.value = root.getItemByName((string)cp.value);
            }
        }


    }






}