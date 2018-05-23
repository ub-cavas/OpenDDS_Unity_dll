using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using test01csharp;



namespace test01csharp
{
    public partial class Form1 : Form
    {
        

        [DllImport("mri_opendds.dll", CallingConvention = CallingConvention.Cdecl)]
        public extern static int getR();

        [DllImport("mri_opendds.dll", CallingConvention = CallingConvention.Cdecl)]
        public extern static float GetDnpwDistance();
        //extern "C" RDTest long GetDnpwDistance();


        [DllImport("mri_opendds.dll", CallingConvention = CallingConvention.Cdecl)]
        public extern static bool start_opendds();

        [DllImport("mri_opendds.dll", CallingConvention = CallingConvention.Cdecl)]
        public extern static void stop_opendds();

        [DllImport("mri_opendds.dll", CallingConvention = CallingConvention.Cdecl)]
        public extern static void getVehs();

        [DllImport("mri_opendds.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public extern static void getVehsArray(out int Num_Vehicles, out IntPtr VehicleData);

        [DllImport("mri_opendds.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public  extern static void updateSubjectCarLocation( float pos_x, float pos_y, float pos_z, float heading, float pitch, float roll, float speed);






        [DllImport("kernel32", SetLastError = true)]
        static extern bool FreeLibrary(IntPtr hModule);


     





        internal static class UnsafeArray
        {
            public static IEnumerable<T> Enumerable<T>(IntPtr pointer, int length) where T : struct
            {
                int sizeInBytes = System.Runtime.InteropServices.Marshal.SizeOf(typeof(T));
                IntPtr p = pointer;
                for (int i = 0; i < length; i++)
                {
                    yield return (T)System.Runtime.InteropServices.Marshal.PtrToStructure(p, typeof(T));
                    p = new IntPtr(p.ToInt64() + sizeInBytes);
                }
            }

            public static T[] ToArray<T>(IntPtr pointer, int length) where T : struct
            {
                return Enumerable<T>(pointer, length).ToArray();
            }

            public static int[] ToArray(IntPtr pointer, int length)
            {
                var array = new int[length];
                Marshal.Copy(pointer, array, 0, length);
                return array;
            }
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct UnityVehicle
        {
            public int timestamp;             //tick every 10 ms*, all applications have to be synchronized ; Mri
            public int vehicle_id;
            public int vehicle_type;              /* vehicle type number from VISSIM */
            public int color;                 /* RGB */
            public float position_x;              /* in m */
            public float position_y;              /* in m */
            public float position_z;              /* in m */
            public float orient_heading;          /* in radians */
            public float orient_pitch;            /* in radians */
            public float orient_roll;             //in radians ; Mri
            public float speed;                   /* in m/s */
            public int leading_vehicle_id;        /* relevant vehicle in front */
            public int trailing_vehicle_id;       /* next vehicle back on the same lane */
            public int link_id;
            public float link_coordinate;         /* in m */
            public int lane_index;                /* 0 = rightmost */
            public int turning_indicator;     /* 1 = left, 0 = none, -1 = right */
        };

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct Mri_VehData
        {
            public int timestamp;
            public int VehicleID;
            public int VehicleType;                         /* vehicle type number from VISSIM */
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
            public string ModelFileName;                    /* *.v3d */
            public int color;                               /* RGB */
            public double Position_X;                       /* in m */
            public double Position_Y;                       /* in m */
            public double Position_Z;                       /* in m */
            public double Orient_Heading;                   /* in radians */
            public double Orient_Pitch;                     /* in radians */
            public double Speed;                            /* in m/s */
            public int LeadingVehicleID;                    /* relevant vehicle in front */
            public int TrailingVehicleID;                   /* next vehicle back on the same lane */
            public int LinkID;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
            public string LinkName;                         /* empty if not set in VISSIM */
            public double LinkCoordinate;                   /* in m */
            public int LaneIndex;                           /* 0 = rightmost */
            public int TurningIndicator;   /* 1 = left, 0 = none, -1 = right */
            //public int PreviousIndex;                       /* for interpolation: index in the array in the previous VISSIM time step, -1 = new in the network */


            //public long timestamp;
            //public long VehicleID;
            //public long VehicleType;                         /* vehicle type number from VISSIM */
            //[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
            //public string ModelFileName;                    /* *.v3d */
            //public long color;                               /* RGB */
            //public double Position_X;                       /* in m */
            //public double Position_Y;                       /* in m */
            //public double Position_Z;                       /* in m */
            //public double Orient_Heading;                   /* in radians */
            //public double Orient_Pitch;                     /* in radians */
            //public double Speed;                            /* in m/s */
            //public long LeadingVehicleID;                    /* relevant vehicle in front */
            //public long TrailingVehicleID;                   /* next vehicle back on the same lane */
            //public long LinkID;
            //[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
            //public string LinkName;                         /* empty if not set in VISSIM */
            //public double LinkCoordinate;                   /* in m */
            //public long LaneIndex;                           /* 0 = rightmost */
            //public long TurningIndicator;   /* 1 = left, 0 = none, -1 = right */



        };

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
           
        }

        void testDictionary ()
        {
            

           // Dictionary<int, Queue<UnityVehicle>> q = new Dictionary<int, Queue<UnityVehicle>>;
            

        }



        private void button1_Click(object sender, EventArgs e)
        {
           
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                bool result = start_opendds();
                if (result==false)
                {
                    MessageBox.Show("Problem with TimeServer");
                }
            }
            catch (Exception er)
            {
                Debug.Print("Error: " + er.Message);
                throw;
            }
            
            button3.Enabled = true;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            stop_opendds();
            button3.Enabled = false;

            //foreach (ProcessModule mod in Process.GetCurrentProcess().Modules)
            //{
            //    if (mod.ModuleName == "mri_opendds.dll")
            //    {
            //        FreeLibrary(mod.BaseAddress);
            //        break;
            //    }
            //}

        }


        public void GetTrafficVehicles(out UnityVehicle[] retVehicleData)
        {
            int vehDataCount = 0;
            IntPtr vehicleData = IntPtr.Zero;
            getVehsArray(out vehDataCount, out vehicleData);
            retVehicleData = UnsafeArray.ToArray<UnityVehicle>(vehicleData, vehDataCount);
        }

        private void button4_Click(object sender, EventArgs e)
        {

            timer1.Enabled = true;

            label1.Text = "Start timer";


            ////getVehs();
            //UnityVehicle[] vehsData = new UnityVehicle[] { };



            //GetTrafficVehicles(out vehsData);
            //string t = "";

            //foreach (var item in vehsData)
            //{
            //    t += "  timestamp: " + item.timestamp + "   vehId=" + item.vehicle_id + " x=" + item.position_x + " y=" + item.position_y + " heading: " + item.orient_heading + "\r\n";

            //}

            //textBox1.Text = t;

        }

        private void button5_Click(object sender, EventArgs e)
        {
            //updateSubjectCarLocation( 47.22f,  -1340.28f, 0f, 45f, 0,0,0);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            //getVehs();
            UnityVehicle[] vehsData = new UnityVehicle[] { };

            try
            {
                GetTrafficVehicles(out vehsData);
                string t = "";

                foreach (var item in vehsData)
                {
                    t += "  timestamp: " + item.timestamp + "   vehId=" + item.vehicle_id + " x=" + item.position_x + " y=" + item.position_y + " heading: " + item.orient_heading + "\r\n";

                }
                textBox1.Text = t;


                updateSubjectCarLocation(-48.829f, 2732.424f, 0f,0.1f, 0f, 0f, 0f);

                float dnpwDistance = GetDnpwDistance();
                label1.Text = dnpwDistance.ToString() + " m";

            }
            catch (Exception )
            {
                label1.Text = "ERROR";
                timer1.Enabled = false;
                Debug.Print("Error");
                //throw ;
            }

           
               
           
        }

        private void button2_Click_1(object sender, EventArgs e)
        {

        }

        private void button1_Click_1(object sender, EventArgs e)
        {

        }

        private void button1_Click_2(object sender, EventArgs e)
        {
            //start
            try
            {
                bool result = start_opendds();
                if (result == false)
                {
                    MessageBox.Show("Problem with TimeServer");
                    
                }
            }
            catch (Exception er)
            {
                Debug.Print("Error: " + er.Message);
                throw;
            }

            button3.Enabled = true;
        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            //stop
            timer1.Enabled = false;
            stop_opendds();
            button3.Enabled = false;
        }

        private void button4_Click_1(object sender, EventArgs e)
        {
            //get

            timer1.Enabled = true;

            label1.Text = "Start timer";
        }

        private void button5_Click_1(object sender, EventArgs e)
        {
            //send
            //updateSubjectCarLocation( 47.22f, -1340.28f, 0f, 45f, 0, 0,0);
        }
    }

    
}
