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

    [StructLayout(LayoutKind.Sequential)]
    public struct NativePakFile
    {
        public UInt64 filenameHash;
        public UInt64 size;
        public UInt64 offset;
        public UInt64 hash;
    }

    public struct PakFile
    {
        public UInt64 filenameHash;
        public UInt64 size;
        public UInt64 offset;
        public UInt64 hash;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct NativePakInfo
    {
        public UInt64 filesCount;
        public UIntPtr files;
    }

    public struct PakInfo
    {
        public UInt64 filesCount;
        public List<PakFile> files;
    }

    public static class PakNative
    {

        [DllImport("lengine.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetPakInfo")]
        private static extern Int32 _NativeGetPakInfo([MarshalAs(UnmanagedType.LPWStr)] String pakPath, IntPtr pakInfo);

        public static PakInfo GetPakInfo(string path)
        {
            NativePakInfo pakInfo = new NativePakInfo();
            IntPtr buffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(pakInfo));
            Marshal.StructureToPtr(pakInfo, buffer, false);
            _NativeGetPakInfo(path, buffer);
            NativePakInfo nativePakInfoRes = (NativePakInfo)Marshal.PtrToStructure(buffer, typeof(NativePakInfo));
            PakInfo pakInfoRes = new PakInfo();
            pakInfoRes.filesCount = nativePakInfoRes.filesCount;
            pakInfoRes.files = new List<PakFile>();

            // Getting files
            for (UInt64 i = 0; i < nativePakInfoRes.filesCount; i++)
            {
                UIntPtr structUPtr = new UIntPtr(nativePakInfoRes.files.ToUInt64() + (32UL * i));
                IntPtr structPtr = unchecked((IntPtr)(long)(ulong)structUPtr);
                NativePakFile nativePakFile = Marshal.PtrToStructure<NativePakFile>(structPtr);
                PakFile pakFile = new PakFile();
                pakFile.filenameHash = nativePakFile.filenameHash;
                pakFile.hash = nativePakFile.hash;
                pakFile.offset = nativePakFile.offset;
                pakFile.size = nativePakFile.size;
                pakInfoRes.files.Add(pakFile);
            }

            // Unable to release memory allocated in a DLL. MEMORY LEAK!
            // Marshal.FreeCoTaskMem(unchecked((IntPtr)(long)(ulong)nativePakInfoRes.files));
            Marshal.FreeCoTaskMem(buffer);

            return pakInfoRes;
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

        private static readonly string[] SizeSuffixes =
                   { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB" };
        public static string SizeSuffix(UInt64 value, int decimalPlaces = 2)
        {
            if (decimalPlaces < 0) { throw new ArgumentOutOfRangeException("decimalPlaces"); }
            if (value == 0) { return string.Format("{0:n" + decimalPlaces + "} bytes", 0); }

            // mag is 0 for bytes, 1 for KB, 2, for MB, etc.
            int mag = (int)Math.Log(value, 1024);

            // 1L << (mag * 10) == 2 ^ (10 * mag) 
            // [i.e. the number of bytes in the unit corresponding to mag]
            decimal adjustedSize = (decimal)value / (1L << (mag * 10));

            // make adjustment when the value is large enough that
            // it would round up to 1000 or more
            if (Math.Round(adjustedSize, decimalPlaces) >= 1000)
            {
                mag += 1;
                adjustedSize /= 1024;
            }

            return string.Format("{0:n" + decimalPlaces + "} {1}",
                adjustedSize,
                SizeSuffixes[mag]);
        }

        public PakInfo mPakInfo { get; private set; }
        public UInt64 NumFiles { get; private set; }

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
            mPakInfo = PakNative.GetPakInfo(nPakPath);
            NumFiles = mPakInfo.filesCount;
        }
    }
}
