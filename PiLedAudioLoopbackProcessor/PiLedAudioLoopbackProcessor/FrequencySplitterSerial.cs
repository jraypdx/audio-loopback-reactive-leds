using System;
using System.Linq;
using System.ComponentModel;
//using System.Net.Sockets;
using System.IO;
using System.IO.Ports;
using MathNet.Numerics;


namespace PiLedAudioLoopbackProcessor
{
    static class FrequencySplitterSerial
    {
        public static double[] processed = new double[9];
        public static double[] processed_ring = new double[24];
        public static double[] processed_60 = new double[60];
        public static double[] lastProcessed = new double[9];
        public static double[] lastProcessed_ring = new double[24];
        public static double[] lastProcessed_60 = new double[24];
        public static double peak = 5.0;
        //public static UdpClient udpClient = new UdpClient();
        public static SerialPort serial = new SerialPort(SerialPort.GetPortNames()[0], 152000, Parity.None, 8, StopBits.One);
        public static bool isOpen = false;



        public static void ProcessBins_60(double[] toSplit) // Testing a new mappaing with 60 bins - didn't work out very well... but leaving in if I want to try again
        {
            //Console.Write(toSplit.Count());
            processed_60 = toSplit.Take(60).ToArray();

            double inverseMax = 1 / processed_60.Max();
            if (processed_60.Max() > peak)
                peak = processed_60.Max();
            else
                peak = peak * 0.9 + processed_60.Max() * 0.1;
            double inversePeak = 1 / peak;

            for (int i = 0; i < 24; i++)
            {
                processed_60[i] = processed_60[i] *= inversePeak;
                if (processed_60[i] > 1.0)
                {
                    //Console.WriteLine(i + "   " + processed_ring[i]);
                    processed_60[i] = 1.0;
                }
                else if (processed_60[i] < lastProcessed_60[i])
                    processed_60[i] = lastProcessed_60[i] *= 0.85;
            }

            lastProcessed_60 = processed_60;
            SendBins(processed_60);
        }

        public static void ProcessBins_RING(double[] toSplit)
        {
            //Console.Write(toSplit.Count());
            //processed_ring = toSplit.Skip(2).Take(24).ToArray();
            int counter = 5;
            processed_ring[0] = toSplit[2] * 0.75; // The sine bass LEDs need a bit of tuning or they overwhelm the rest
            processed_ring[1] = toSplit[3] * 0.825;
            processed_ring[2] = toSplit[4] * 0.9;
            processed_ring[3] = toSplit[5] * 0.925;
            processed_ring[4] = toSplit[6] * 0.975;
            for (int i = 7; i < 80; i++)
            {
                if (counter > 23) { break; }
                if (i >= 20)
                {
                    processed_ring[counter] = (toSplit[i] + toSplit[i + 1] + toSplit[i + 2]) * 0.33333;
                    i += 2;
                }
                else if (i >= 12)
                {
                    processed_ring[counter] = (toSplit[i] + toSplit[i + 1]) * 0.5;
                    i++;
                }
                else
                    processed_ring[counter] = toSplit[i];
                counter++;
            }

            double inverseMax = 1 / processed_ring.Max();
            if (processed_ring.Max() > peak)
                peak = processed_ring.Max();
            else
                peak = peak * 0.95 + processed_ring.Max() * 0.05;
            double inversePeak = 1 / peak;

            for (int i = 0; i < 24; i++)
            {
                processed_ring[i] = processed_ring[i] *= inversePeak;
                if (processed_ring[i] > 1.0)
                {
                    //Console.WriteLine(i + "   " + processed_ring[i]);
                    processed_ring[i] = 1.0;
                }
                else if (processed_ring[i] < 0.1)
                    processed_ring[i] = 0.0;
                else if (processed_ring[i] < lastProcessed_ring[i])
                    processed_ring[i] = lastProcessed_ring[i] *= 0.8;// + processed_ring[i] * 0.1;
            }
            
            lastProcessed_ring = processed_ring;
            SendBins(processed_ring);
        }

        public static void ProcessBins(double[] toSplit)
        {
            if (toSplit.Sum() == 0)
                return;
            //if (toSplit.Sum() > 0)
            //{
            //    double inverseMax = 1 / toSplit.Max();
            //    for (int i = 0; i < 9; i++)
            //    {
            //        toSplit[i] = toSplit[i] * inverseMax;
            //    }
            //}

            processed[0] = (toSplit[1] + toSplit[2]) * 0.5;// * 1.8;
            processed[1] = (toSplit[2] + toSplit[3]) * 0.5;// * 1.5;
            processed[2] = (toSplit[3] + toSplit[4]) * 0.5;// * 0.65;
            processed[3] = (toSplit[4] + toSplit[5]) * 0.5;
            processed[4] = (toSplit.Skip(5).Take(3)).Average();
            processed[5] = (toSplit.Skip(7).Take(4)).Average();
            processed[6] = (toSplit.Skip(11).Take(5)).Average();
            processed[7] = (toSplit.Skip(16).Take(6)).Average();
            processed[8] = (toSplit.Skip(22).Take(10)).Average();
            //Console.WriteLine(processed[8]);

            //double inverseMax = 1 / processed.Max();
            //for (int i = 0; i < 9; i++)
            //{
            //    processed[i] = Math.Pow((processed[i] * 0.5) + (processed[i] * 0.5 * inverseMax), 0.9);
            //}
            //inverseMax = 1 / processed.Max();
            //for (int i = 0; i < 9; i++)
            //{
            //    processed[i] *= inverseMax;
            //}

            if (processed.Max() > peak)
                peak = processed.Max();
            else
                peak *= 0.95;

            double inversePeak = 1 / peak;
            //for (int i = 0; i < 9; i++)
            //{
            //    processed[i] = processed[i] *= inversePeak;
            //}

            for (int i = 0; i < 9; i++)
            {
                processed[i] = processed[i] *= inversePeak;
                if (processed[i] > 1.0)
                {
                    Console.WriteLine(i + "   " + processed[i]);
                    processed[i] = 1.0;
                }
                //else if (processed[i] < 0.1)
                //{
                //    processed[i] = lastProcessed[i] * 0.9;
                //}
                else if (processed[i] < lastProcessed[i])
                    processed[i] = lastProcessed[i] *= 0.9;
            }

            //debugging print
            //foreach (var p in processed)
            //{
            //    Console.Write(p + "   ");
            //}
            //Console.WriteLine("---");

            lastProcessed = processed;
            SendBins(processed);
        }

        public static void SendBins(double[] toSend)
        {
            if (!isOpen)
            {
                //Console.WriteLine(SerialPort.GetPortNames().Length);
                //foreach (var sp in SerialPort.GetPortNames())
                //    Console.WriteLine(sp);
                serial.Open();
                isOpen = true;
                //serial.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);// (data, err) => { Console.WriteLine(data); };
            }

            //foreach (var d in toSend)
            //    Console.WriteLine(d.ToString());
            //Console.WriteLine("");
            int numBytes = toSend.Length;// * 4;// 8;
            byte[] data = new byte[numBytes];
            for (int i = 0; i < numBytes; i++)
            {
                data[i] = (byte)((int)(toSend[i] * 255));
            }
            data = data.Reverse().ToArray(); // Reverse here?  Idk I did the colors in the arduino code backwards apparently and it's easier to fix here
            //float[] toSendFloats = Array.ConvertAll(toSend, Convert.ToSingle); // Arduino (seeeduino xiao) board doesn't use doubles, only floats I guess
            //byte[] data = toSendFloats.SelectMany(x => BitConverter.GetBytes(x)).ToArray();
            //udpClient.SendAsync(data, numBytes, "192.168.69.40", 6969);
            serial.Write(data, 0, numBytes);
            //foreach (var b in data)
            //    Console.Write(b.ToString() + " ");
            //Console.WriteLine("");
            //Console.WriteLine("");
            //serial.Write("test");
            //serial.Read(data, 0, numBytes);
            //Console.WriteLine(data.ToString());
        }

        //private static void DataReceivedHandler(
        //                object sender,
        //                SerialDataReceivedEventArgs e)
        //{
        //    SerialPort sp = (SerialPort)sender;
        //    string indata = sp.ReadExisting();
        //    Console.WriteLine("Data Received:");
        //    Console.Write(indata);
        //}

        //public static void PrintSerialDebugging()
    }
}

/*
 * Using 32 frequency bins/sections by default
 * 
 * From testing with gui, found this (approximate) data:
 * 
 * bin  |  approx freq
 * 0,1     none - ignore these two
 *  2      30hz (1/10 vol)  25-27, 42-44, 49-51, 110-112, 117-119, 93-95 ----this one too low, shift all up one and redistribute more for top band???
 * 2-3     40hz (1/5 vol)   23-24, 28-29, 40-41, 47-48, 108-109, 115-116, 96-97, 91-92
 *  3      50hz (2/5 vol)   30-31, 22, 38-39, 45-46, 113-114, 106-107, 98-99, 90-91
 * 3-4     60hz (3/5 vol)   32-33, 20-21, 38, 45, 100-101, 88-89
 * 4-5     80hz (2/3 vol)   34-35, 18-19, 102-103, 86-87
 * 5-7     100hz - 120hz    36-37, 16-17, 104-105, 84-85
 * 7-11    140hz -180hz     5-6, 14-15, 3-4, 52-53, 62, 71-74, 82-83, 120-121
 * 11-16   200-300hz        7-8, 12-13, 2, 54, 61, 63, 122, 70, 75-76, 80-81
 * 16-23   320-440hz        9, 11, 1, 55, 60, 64, 123, 69, 77, 79
 * 23-31   460-600hz        10, 0, 56-59, 65-68, 124, 78
 * 
 */
