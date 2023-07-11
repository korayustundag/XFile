using System;

namespace libXFile
{
	[Serializable]
	public class SourceFileIsNotXFileException : Exception
	{
		public SourceFileIsNotXFileException() { }
		public SourceFileIsNotXFileException(string message) : base(message) { }
		public SourceFileIsNotXFileException(string message, Exception inner) : base(message, inner) { }
		protected SourceFileIsNotXFileException(
		  System.Runtime.Serialization.SerializationInfo info,
		  System.Runtime.Serialization.StreamingContext context) : base(info, context) { }
	}
}
