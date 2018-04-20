using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;



namespace PTV.Vision.Interfaces
{

  //==================
  // type definitions
  //==================

  public enum TurningIndicatorType
  {
    TurningIndicatorLeft = 1,
    TurningIndicatorNone = 0,
    TurningIndicatorRight = -1
  };



  public enum SignalStateType
  {
    SignalStateRed = 1,
    SignalStateRedAmber = 2,
    SignalStateGreen = 3,
    SignalStateAmber = 4,
    SignalStateOff = 5,
    SignalStateUndefined = 6,
    SignalStateFlashingAmber = 7,
    SignalStateFlashingRed = 8,
    SignalStateFlashingGreen = 9,
    SignalStateAlternatingRedGreen = 10,
    SignalStateGreenAmber = 11
  };



  [StructLayout(LayoutKind.Sequential)]
  public struct Simulator_Veh_Data
  {
    public double Position_X;     /* in m */
    public double Position_Y;     /* in m */
    public double Position_Z;     /* in m */
    public double Orient_Heading; /* in radians */
    public double Orient_Pitch;   /* in radians */
    public double Speed;          /* in m/s */
  };



  [StructLayout(LayoutKind.Sequential)]
  public struct Simulator_Ped_Data
  {
    public double Position_X;             /* in m */
    public double Position_Y;             /* in m */
    public double Position_Z;             /* in m */
    public double Orient_Heading;         /* in radians */
    public double DistanceSinceBirth;     /* in m */
    public double Speed;                  /* in m/s */
  };



  [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
  public struct VISSIM_Veh_Data
  {
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
    public TurningIndicatorType TurningIndicator;   /* 1 = left, 0 = none, -1 = right */
    public int PreviousIndex;                       /* for interpolation: index in the array in the previous VISSIM time step, -1 = new in the network */
  };



  public enum Pedestrian_Construction_Element_Type
  {
    PedestrianConstructionElementTypeNone,
    PedestrianConstructionElementTypeArea,
    PedestrianConstructionElementTypeRamp,
    PedestrianConstructionElementTypeElevatorGroup,
    PedestrianConstructionElementTypePedLink
  };



  public enum Pedestrian_Motion_State_Type
  {
    Motion_State_Type_ApproachingPTVehicle = 1,
    Motion_State_Type_AlightingFromPTVehicle = 2,
    Motion_State_Type_WaitingForPTVehicle = 3,
    Motion_State_Type_WalkingUpOnEscalator = 4,
    Motion_State_Type_WalkingDownOnEscalator = 5,
    Motion_State_Type_StandingOnEscalator = 6,
    Motion_State_Type_WalkingOnMovingWalkway = 7,
    Motion_State_Type_StandingOnMovingWalkway = 8,
    Motion_State_Type_ServicedAtQueueHead = 9,
    Motion_State_Type_WaitingInQueue = 10,
    Motion_State_Type_WalkingUpstairs = 11,
    Motion_State_Type_WalkingDownstairs = 12,
    Motion_State_Type_ApproachingElevator = 13,
    Motion_State_Type_AlightingFromElevator = 14,
    Motion_State_Type_WaitingForElevator = 15,
    Motion_State_Type_RidingElevator = 16,
    Motion_State_Type_Waiting = 17,
    Motion_State_Type_WalkingOnLevel = 18,
    Motion_State_Type_End = 19
  };



  [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
  public struct VISSIM_Ped_Data
  {
    public int PedestrianID;
    public int PedestrianType;                                             /* pedestrian type number from VISSIM */
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
    public string ModelFileName;                                           /* *.v3d */
    public double Length;                                                  /* in m */
    public double Width;                                                   /* in m */
    public double Height;                                                  /* in m */
    public double Position_X;                                              /* in m */
    public double Position_Y;                                              /* in m */
    public double Position_Z;                                              /* in m */
    public double Orient_Heading;                                          /* in radians */
    public double Orient_Pitch;                                            /* in radians */
    public double DistanceSinceBirth;                                      /* in m */
    public double Speed;                                                   /* in m/s */
    public Pedestrian_Motion_State_Type MotionState;                       /* the current motion state */
    public Pedestrian_Construction_Element_Type ConstructionElementType;   /* the type of the construction element */
    public int ConstructionElementID;                                      /* the construction element the pedestrian is currently on */
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
    public string ConstructionElementName;                                 /* empty if not set in VISSIM */
    public int PreviousIndex;                                              /* for interpolation: index in the array in the previous VISSIM time step, -1 = new in the network */
  };



  [StructLayout(LayoutKind.Sequential)]
  public struct VISSIM_Sig_Data
  {
    public int ControllerID;
    public int SignalGroupID;
    public SignalStateType SignalState;
  };

  //=========================================================
  // convenience class to map structs onto raw memory blocks
  // ========================================================

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

  //==============================================
  // interface for the simulator
  //==============================================

  public class DrivingSimulatorInterface : IDisposable
  {
    public bool Connected { get; private set; }

    //=============
    // dll imports
    //=============

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern bool VISSIM_Connect(int versionNo
      , [MarshalAs(UnmanagedType.LPWStr)] string networkFileName
      , int simulatorFrequency
      , double visibilityRadius
      , int maxSimulatorVeh
      , int maxSimulatorPed
      , int maxSimulatorDet
      , int maxVissimVeh
      , int maxVissimPed
      , int maxVissimSigGrp);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern bool VISSIM_ConnectToConsole([MarshalAs(UnmanagedType.LPWStr)] string consoleFileName
      , [MarshalAs(UnmanagedType.LPWStr)] string networkFileName
      , int simulatorFrequency
      , double visibilityRadius
      , int maxSimulatorVeh
      , int maxSimulatorPed
      , int maxSimulatorDet
      , int maxVissimVeh
      , int maxVissimPed
      , int maxVissimSigGrp);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern bool VISSIM_Disconnect();

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern bool VISSIM_SetDriverVehicles(int Num_Vehicles, IntPtr vehdata);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern bool VISSIM_SetDriverPedestrians(int Num_Pedestrians, IntPtr peddata);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern bool VISSIM_SetDriverVehiclesAndPedestrians(int Num_Vehicles, IntPtr vehdata
      , int Num_Pedestrians, IntPtr peddata);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void VISSIM_SetDetection(int DetectorID, int ControllerID);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern bool VISSIM_DataReady();

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void VISSIM_GetTrafficVehicles(out int Num_Vehicles, out IntPtr VehicleData);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void VISSIM_GetTrafficPedestrians(out int Num_Pedestrians, out IntPtr PedestrianData);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void VISSIM_GetVehicleLists(out int NumNewVehicles, out IntPtr NewVehicleIds, out IntPtr NewVehType
      , out int NumMovedVehicles, out IntPtr MovedVehicleIds
      , out int NumDeletedVehicles, out IntPtr DeletedVehicleIds);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void VISSIM_GetPedestrianLists(out int NumNewPedestrians, out IntPtr NewPedestrianIds, out IntPtr NewPedType
      , out int NumMovedPedestrians, out IntPtr MovedPedestrianIds
      , out int NumDeletedPedestrians, out IntPtr DeletedPedestrianIds);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void VISSIM_GetSignalStates(out int NumSignals, out IntPtr SignalStateData);

    [DllImport("DrivingSimulatorProxy", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern string GetLastErrorMessage();

    //=======================================
    // connect with normal VISSIM (with GUI)
    //=======================================

    public DrivingSimulatorInterface(int version
      , string networkFileName
      , int simulatorFrequency
      , double visibilityRadius
      , int maxSimulatorVeh
      , int maxSimulatorPed
      , int maxSimulatorDet
      , int maxVissimVeh
      , int maxVissimPed
      , int maxVissimSigGrp)
    {
      if (!Connect(version
        , networkFileName
        , simulatorFrequency
        , visibilityRadius
        , maxSimulatorVeh
        , maxSimulatorPed
        , maxSimulatorDet
        , maxVissimVeh
        , maxVissimPed
        , maxVissimSigGrp))
      {
        throw new Exception("Connection has failed.");
      }
    }

    public bool Connect(int version
      , string networkFileName
      , int simulatorFrequency
      , double visibilityRadius
      , int maxSimulatorVeh
      , int maxSimulatorPed
      , int maxSimulatorDet
      , int maxVissimVeh
      , int maxVissimPed
      , int maxVissimSigGrp)
    {
      this.Connected = VISSIM_Connect(version
        , networkFileName
        , simulatorFrequency
        , visibilityRadius
        , maxSimulatorVeh
        , maxSimulatorPed
        , maxSimulatorDet
        , maxVissimVeh
        , maxVissimPed
        , maxVissimSigGrp);
      return this.Connected;
    }

    //=============================
    // connect with console VISSIM
    //=============================

    public DrivingSimulatorInterface(string consoleFileName
      , string networkFileName
      , int simulatorFrequency
      , double visibilityRadius
      , int maxSimulatorVeh
      , int maxSimulatorPed
      , int maxSimulatorDet
      , int maxVissimVeh
      , int maxVissimPed
      , int maxVissimSigGrp)
    {
      if (!ConnectConsole(consoleFileName
        , networkFileName
        , simulatorFrequency
        , visibilityRadius
        , maxSimulatorVeh
        , maxSimulatorPed
        , maxSimulatorDet
        , maxVissimVeh
        , maxVissimPed
        , maxVissimSigGrp))
      {
        throw new Exception("Connection has failed.");
      }
    }

    public bool ConnectConsole(string consoleFileName
      , string networkFileName
      , int simulatorFrequency
      , double visibilityRadius
      , int maxSimulatorVeh
      , int maxSimulatorPed
      , int maxSimulatorDet
      , int maxVissimVeh
      , int maxVissimPed
      , int maxVissimSigGrp)
    {
      this.Connected = VISSIM_ConnectToConsole(consoleFileName
        , networkFileName
        , simulatorFrequency
        , visibilityRadius
        , maxSimulatorVeh
        , maxSimulatorPed
        , maxSimulatorDet
        , maxVissimVeh
        , maxVissimPed
        , maxVissimSigGrp);
      return this.Connected;
    }

    //========================
    // disconnect from VISSIM
    //========================

    public bool Disconnect()
    {
      this.Connected = !VISSIM_Disconnect();
      return this.Connected;
    }

    public bool DataReady()
    {
      return VISSIM_DataReady();
    }

    public string GetLastError()
    {
      return GetLastErrorMessage();
    }

    //========================================
    // send data from the simulator to VISSIM
    //========================================

    public void SetDriverVehicles(Simulator_Veh_Data[] vehData)
    {
      IntPtr data = Marshal.AllocHGlobal(vehData.Length * Marshal.SizeOf(typeof(Simulator_Veh_Data)));
      for (int i = 0; i < vehData.Length; ++i)
      {
        Marshal.StructureToPtr(vehData[i], new IntPtr(data.ToInt64() + i * Marshal.SizeOf(typeof(Simulator_Veh_Data))), false);
      }
      VISSIM_SetDriverVehicles(vehData.Length, data);
    }

    public void SetDriverPedestrians(Simulator_Ped_Data[] pedData)
    {
      IntPtr pedDataMarshalled = Marshal.AllocHGlobal(pedData.Length * Marshal.SizeOf(typeof(Simulator_Ped_Data)));
      for (int i = 0; i < pedData.Length; ++i)
      {
        Marshal.StructureToPtr(pedData[i], new IntPtr(pedDataMarshalled.ToInt64() + i * Marshal.SizeOf(typeof(Simulator_Ped_Data))), false);
      }
      VISSIM_SetDriverPedestrians(pedData.Length, pedDataMarshalled);
    }

    public void SetDriverVehiclesAndPedestrians(Simulator_Veh_Data[] vehData, Simulator_Ped_Data[] pedData)
    {
      IntPtr vehDataMarshalled = Marshal.AllocHGlobal(vehData.Length * Marshal.SizeOf(typeof(Simulator_Veh_Data)));
      for (int i = 0; i < vehData.Length; ++i)
      {
        Marshal.StructureToPtr(vehData[i], new IntPtr(vehDataMarshalled.ToInt64() + i * Marshal.SizeOf(typeof(Simulator_Veh_Data))), false);
      }
      IntPtr pedDataMarshalled = Marshal.AllocHGlobal(pedData.Length * Marshal.SizeOf(typeof(Simulator_Ped_Data)));
      for (int i = 0; i < pedData.Length; ++i)
      {
        Marshal.StructureToPtr(pedData[i], new IntPtr(pedDataMarshalled.ToInt64() + i * Marshal.SizeOf(typeof(Simulator_Ped_Data))), false);
      }
      VISSIM_SetDriverVehiclesAndPedestrians(vehData.Length, vehDataMarshalled, pedData.Length, pedDataMarshalled);
    }

    public void SetDetection(int DetectorID, int ControllerID)
    {
      VISSIM_SetDetection(DetectorID, ControllerID);
    }

    //==========================
    // receive data from VISSIM
    //==========================

    public void GetVehicleLists(out int[] newIds, out int[] newVehTypes, out int[] movedIds, out int[] deletedIds)
    {
      int numnew = 0;
      IntPtr newids = IntPtr.Zero;
      IntPtr newvehtypes = IntPtr.Zero;
      int nummoved = 0;
      int numdeleted = 0;
      IntPtr deletedids = IntPtr.Zero;
      IntPtr movedids = IntPtr.Zero;

      VISSIM_GetVehicleLists(out numnew, out newids, out newvehtypes, out nummoved, out movedids, out numdeleted, out deletedids);

      newIds = UnsafeArray.ToArray(newids, numnew);
      newVehTypes = UnsafeArray.ToArray(newvehtypes, numnew);
      movedIds = UnsafeArray.ToArray(movedids, nummoved);
      deletedIds = UnsafeArray.ToArray(deletedids, numdeleted);
    }

    public void GetPedestrianLists(out int[] newPedestrianIds, out int[] newPedestrianTypes, out int[] movedPedestrianIds, out int[] deletedPedestrianIds)
    {
      int numNewPedestriansRaw = 0;
      IntPtr newPedestrianIdsRaw = IntPtr.Zero;
      IntPtr newPedestriantypesRaw = IntPtr.Zero;
      int numMovedPedestriansRaw = 0;
      int numDeletedPedestriansRaw = 0;
      IntPtr deletedPedestrianIdsRaw = IntPtr.Zero;
      IntPtr movedPedestrianIdsRaw = IntPtr.Zero;

      VISSIM_GetPedestrianLists(out numNewPedestriansRaw, out newPedestrianIdsRaw, out newPedestriantypesRaw, out numMovedPedestriansRaw, out movedPedestrianIdsRaw, out numDeletedPedestriansRaw, out deletedPedestrianIdsRaw);

      newPedestrianIds = UnsafeArray.ToArray(newPedestrianIdsRaw, numNewPedestriansRaw);
      newPedestrianTypes = UnsafeArray.ToArray(newPedestriantypesRaw, numNewPedestriansRaw);
      movedPedestrianIds = UnsafeArray.ToArray(movedPedestrianIdsRaw, numMovedPedestriansRaw);
      deletedPedestrianIds = UnsafeArray.ToArray(deletedPedestrianIdsRaw, numDeletedPedestriansRaw);
    }

    public void GetVehicleAndPedestrianLists(out int[] newVehicleIds, out int[] newVehicleTypes, out int[] movedVehicleIds, out int[] deletedVehicleIds, out int[] newPedestrianIds, out int[] newPedestrianTypes, out int[] movedPedestrianIds, out int[] deletedPedestrianIds)
    {
      int numNewVehiclesRaw = 0;
      IntPtr newVehicleIdsRaw = IntPtr.Zero;
      IntPtr newVehicletypesRaw = IntPtr.Zero;
      int numMovedVehiclesRaw = 0;
      int numDeletedVehiclesRaw = 0;
      IntPtr deletedVehicleIdsRaw = IntPtr.Zero;
      IntPtr movedVehicleIdsRaw = IntPtr.Zero;

      VISSIM_GetVehicleLists(out numNewVehiclesRaw, out newVehicleIdsRaw, out newVehicletypesRaw, out numMovedVehiclesRaw, out movedVehicleIdsRaw, out numDeletedVehiclesRaw, out deletedVehicleIdsRaw);
      newVehicleIds = UnsafeArray.ToArray(newVehicleIdsRaw, numNewVehiclesRaw);
      newVehicleTypes = UnsafeArray.ToArray(newVehicletypesRaw, numNewVehiclesRaw);
      movedVehicleIds = UnsafeArray.ToArray(movedVehicleIdsRaw, numMovedVehiclesRaw);
      deletedVehicleIds = UnsafeArray.ToArray(deletedVehicleIdsRaw, numDeletedVehiclesRaw);

      int numNewPedestriansRaw = 0;
      IntPtr newPedestrianIdsRaw = IntPtr.Zero;
      IntPtr newPedestriantypesRaw = IntPtr.Zero;
      int numMovedPedestriansRaw = 0;
      int numDeletedPedestriansRaw = 0;
      IntPtr deletedPedestrianIdsRaw = IntPtr.Zero;
      IntPtr movedPedestrianIdsRaw = IntPtr.Zero;

      VISSIM_GetPedestrianLists(out numNewPedestriansRaw, out newPedestrianIdsRaw, out newPedestriantypesRaw, out numMovedPedestriansRaw, out movedPedestrianIdsRaw, out numDeletedPedestriansRaw, out deletedPedestrianIdsRaw);
      newPedestrianIds = UnsafeArray.ToArray(newPedestrianIdsRaw, numNewPedestriansRaw);
      newPedestrianTypes = UnsafeArray.ToArray(newPedestriantypesRaw, numNewPedestriansRaw);
      movedPedestrianIds = UnsafeArray.ToArray(movedPedestrianIdsRaw, numMovedPedestriansRaw);
      deletedPedestrianIds = UnsafeArray.ToArray(deletedPedestrianIdsRaw, numDeletedPedestriansRaw);
    }

    public void GetTrafficeVehicles(out VISSIM_Veh_Data[] retVehicleData)
    {
      int vehDataCount = 0;
      IntPtr vehicleData = IntPtr.Zero;
      VISSIM_GetTrafficVehicles(out vehDataCount, out vehicleData);
      retVehicleData = UnsafeArray.ToArray<VISSIM_Veh_Data>(vehicleData, vehDataCount);
    }

    public void GetTrafficePedestrians(out VISSIM_Ped_Data[] retPedestrianData)
    {
      int pedestrianDataCount = 0;
      IntPtr pedestrianData = IntPtr.Zero;
      VISSIM_GetTrafficPedestrians(out pedestrianDataCount, out pedestrianData);
      retPedestrianData = UnsafeArray.ToArray<VISSIM_Ped_Data>(pedestrianData, pedestrianDataCount);
    }

    public void GetTrafficeVehiclesAndPedestrians(out VISSIM_Veh_Data[] retVehicleData, out VISSIM_Ped_Data[] retPedestrianData)
    {
      int vehicleDataCount = 0;
      IntPtr vehicleData = IntPtr.Zero;
      VISSIM_GetTrafficVehicles(out vehicleDataCount, out vehicleData);
      retVehicleData = UnsafeArray.ToArray<VISSIM_Veh_Data>(vehicleData, vehicleDataCount);

      int pedestrianDataCount = 0;
      IntPtr pedestrianData = IntPtr.Zero;
      VISSIM_GetTrafficPedestrians(out pedestrianDataCount, out pedestrianData);
      retPedestrianData = UnsafeArray.ToArray<VISSIM_Ped_Data>(pedestrianData, pedestrianDataCount);
    }

    public void GetSignalStates(out VISSIM_Sig_Data[] retSigData)
    {
      int signalDataCount = 0;
      IntPtr sigData = IntPtr.Zero;
      VISSIM_GetSignalStates(out signalDataCount, out sigData);
      retSigData = UnsafeArray.ToArray<VISSIM_Sig_Data>(sigData, signalDataCount);
    }

    #region IDisposable Support

    private bool disposedValue = false;

    protected virtual void Dispose(bool disposing)
    {
      if (!disposedValue)
      {
        if (this.Connected)
        {
          Disconnect();
        }

        disposedValue = true;
      }
    }

    ~DrivingSimulatorInterface()
    {
      Dispose(false);
    }

    public void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    }
    #endregion
  }
}
