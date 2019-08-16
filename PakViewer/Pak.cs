using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace PakViewer
{

    public static class PakNative
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct PakFile
        {
            public UInt64 filenameHash;
            public UInt64 size;
            public UInt64 offset;
            public UInt64 hash;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct PakInfo
        {
            public UInt64 filesCount;
            public PakFile[] files;
        }
    }

    public class PakBadImageException : Exception
    {
        public PakBadImageException()
        {
        }
    }

    public class Pak
    {
        public class PakFile
        {
            public String FileName;
            public UInt64 FileNameHash;
            public UInt64 Size;
            public UInt64 Offset;
            public UInt64 Hash;
        }


        public Pak(string nPakPath)
        {
            
        }
    }
}
