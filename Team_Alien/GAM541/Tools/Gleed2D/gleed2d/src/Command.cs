using System;
using System.Collections.Generic;

namespace GLEED2D
{
    enum CommandType
    {
        Transform, Add, Delete, NameChange, OrderChange, Wholeroot
    }

    class Command
    {
        public String Description;
        public CommandType ComType;
        public List<IUndoable> ObjectsBefore = new List<IUndoable>();
        public List<IUndoable> ObjectsAfter = new List<IUndoable>();

        public Command(string description)
        {
            ComType = CommandType.Wholeroot;
            Description = description;
            ObjectsBefore.Add(Editor.Instance.root.cloneforundo());
        }

        public List<IUndoable> Undo()
        {
            switch (ComType)
            {
                case CommandType.Wholeroot:
                    Editor.Instance.root = (root)ObjectsBefore[0];
                    Editor.Instance.getSelectionFromroot();
                    Editor.Instance.updatetreeview();
                    break;
            }
            return null;
        }

        public List<IUndoable> Redo()
        {
            switch (ComType)
            {
                case CommandType.Wholeroot:
                    Editor.Instance.root = (root)ObjectsAfter[0];
                    Editor.Instance.getSelectionFromroot();
                    Editor.Instance.updatetreeview();
                    break;
            }
            return null;
        }

        public void saveAfterState()
        {
            ObjectsAfter.Add(Editor.Instance.root.cloneforundo());
        }

    }
}
