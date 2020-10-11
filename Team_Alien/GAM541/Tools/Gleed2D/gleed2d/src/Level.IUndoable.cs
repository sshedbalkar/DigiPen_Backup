using System;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Serialization;
using System.Diagnostics;
using System.Windows.Forms;


namespace GLEED2D
{
    [DebuggerDisplay("Name: {Name}, Address: {&Name}")]
    public partial class root : IUndoable
    {

        public void add()
        {
        }

        public void remove()
        {
        }

        public IUndoable cloneforundo()
        {
            selecteditems = "";
            foreach (Item i in Editor.Instance.SelectedItems) selecteditems += i.Name + ";";
            if (Editor.Instance.SelectedLayer != null) selectedlayers = Editor.Instance.SelectedLayer.Name;


            root result = (root)this.MemberwiseClone();
            result.Level = new Level("Level");
            result.Level = result.Level.clone();
            result.Level.root = result;
            return (IUndoable)result;
        }

        public void makelike(IUndoable other)
        {
            /*Layer l2 = (Layer)other;
            Items = l2.Items;
            treenode.Nodes.Clear();
            foreach (Item i in Items)
            {
                Editor.Instance.addItem(i);
            }*/


            root l2 = (root)other;
            Level = l2.Level;
            treenode.Nodes.Clear();

            Editor.Instance.addLevel(Level);
        }

        public string getName()
        {
            return Name;
        }

        public void setName(string name)
        {
            Name = name;
            treenode.Text = name;
        }

    }
}
