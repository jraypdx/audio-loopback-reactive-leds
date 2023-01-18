using System;

namespace PiLedAudioLoopbackProcessor
{
    internal class Program
    {
        static void Main(string[] args)
        {
            FrequencyProcessor freq = new FrequencyProcessor();
            freq.Load();
        }
    }
}
