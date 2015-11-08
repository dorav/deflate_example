using System;
using System.IO;
using System.IO.Compression;
using System.Text;

namespace TestApp
{
	class Program
	{
		static UnicodeEncoding uniEncoding = new UnicodeEncoding();
		
		public static byte[] Compress(string str, Encoding enc)
		{
	        using (MemoryStream input = new MemoryStream(enc.GetBytes(str)))
			using (var dstStream = new MemoryStream(100))
			{
				using (var compressionStream = new DeflateStream(dstStream, CompressionMode.Compress))
				{
					input.CopyTo(compressionStream);
				}

				return dstStream.ToArray();
	        }
		}
		
		public static byte[] Decompress(byte[] compressedData)
		{
			using (MemoryStream compressedStream = new MemoryStream(compressedData))
            using (MemoryStream decompressedStream = new MemoryStream())
			using (DeflateStream decompressionStream = new DeflateStream(compressedStream, CompressionMode.Decompress))
			{
				decompressionStream.CopyTo(decompressedStream);
				return decompressedStream.GetBuffer();
			}
		}
		
		static int Main()
		{
			string original = "hello world from c#";
			byte[] compressed = Compress(original, uniEncoding);
			Console.WriteLine("Compressed data has " + compressed.Length + " bytes");
			byte[] decompressed = Decompress(compressed);
			
			string output = uniEncoding.GetString(decompressed);
			 
			Console.WriteLine("Compressed the string: " + original);
			Console.WriteLine("Into the string      : " + output);
			return 0;	
		}
	}
}