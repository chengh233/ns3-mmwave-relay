 /* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
 /*
 *   Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *   Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
 *   Copyright (c) 2017, ANDSL, School of Electrical and Computer Engineering, Georgia Tech
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   Author: Marco Miozzo <marco.miozzo@cttc.es>
 *           Nicola Baldo  <nbaldo@cttc.es>
 *
 *   Modified by: Marco Mezzavilla < mezzavilla@nyu.edu>
 *        	 	  Sourjya Dutta <sdutta@nyu.edu>
 *        	 	  Russell Ford <russell.ford@nyu.edu>
 *        		  Menglei Zhang <menglei@nyu.edu>
 *
 *        		  Qiang Hu <qianghu@gatech.edu>
 *
 */

#include <ns3/buildings-module.h>
#include "ns3/mmwave-helper.h"
#include "ns3/lte-module.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/mmwave-point-to-point-epc-helper.h"
//#include "ns3/gtk-config-store.h"

using namespace ns3;

/*
 * This program is to simulate a simple two hop wireless relaying path in the mmWave cellular network:
 * 1 mmwave-enb node plays as an end node of the relaying path which connects to the SGW/PGW of the LTE EPC.
 * 1 mmwave-iab node plays as a relay node in the middle which connects to the mmwave-enb node.
 * 1 mmwave-ue node plays as the other end node of the relaying path, which connects to the mmwave-iab node.
 *
 * In this scenario, we are testing the fundamental ideas proposed in our papers.
 *
 */

/*
 * Define a logging component "MmWaveTwoHopRelaying".
 */
NS_LOG_COMPONENT_DEFINE ("MmWaveTwoHopRelaying");

/*
 * Print a list of buildings to file.
 */
void 
PrintGnuplottableBuildingListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  uint32_t index = 0;
  for (BuildingList::Iterator it = BuildingList::Begin (); it != BuildingList::End (); ++it)
    {
      ++index;
      Box box = (*it)->GetBoundaries ();
      outFile << "set building object " << index
              << " rect. from " << box.xMin  << "," << box.yMin
              << " to "   << box.xMax  << "," << box.yMax
              //<< " height " << box.zMin << "," << box.zMax
              << " front fs empty "
              << std::endl;
    }
}

/*
 * This function prints three types of ues to file.
 */
void 
PrintGnuplottableUeListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<LteUeNetDevice> uedev = node->GetDevice (j)->GetObject <LteUeNetDevice> ();
          Ptr<MmWaveUeNetDevice> mmuedev = node->GetDevice (j)->GetObject <MmWaveUeNetDevice> ();
          Ptr<McUeNetDevice> mcuedev = node->GetDevice (j)->GetObject <McUeNetDevice> ();
          if (uedev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << uedev->GetImsi ()
                      << "\" at "<< pos.x << "," << pos.y << " left font \"Helvetica,8\" textcolor rgb \"black\" front point pt 1 ps 0.3 lc rgb \"black\" offset 0,0"
                      << std::endl;
            }
          else if (mmuedev)
           {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << mmuedev->GetImsi ()
                      << "\" at "<< pos.x << "," << pos.y << " left font \"Helvetica,8\" textcolor rgb \"black\" front point pt 1 ps 0.3 lc rgb \"black\" offset 0,0"
                      << std::endl;
            }
          else if (mcuedev)
           {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << mcuedev->GetImsi ()
                      << "\" at "<< pos.x << "," << pos.y << " left font \"Helvetica,8\" textcolor rgb \"black\" front point pt 1 ps 0.3 lc rgb \"black\" offset 0,0"
                      << std::endl;
            } 
        }
    }
}

/*
 * This function prints enb info to file, including lte enb, mmwave enb, and mmwave iab.
 */
void 
PrintGnuplottableEnbListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<LteEnbNetDevice> enbdev = node->GetDevice (j)->GetObject <LteEnbNetDevice> ();
          Ptr<MmWaveEnbNetDevice> mmdev = node->GetDevice (j)->GetObject <MmWaveEnbNetDevice> ();
          Ptr<MmWaveIabNetDevice> mmIabdev = node->GetDevice (j)->GetObject <MmWaveIabNetDevice> ();

          if (enbdev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << enbdev->GetCellId ()
                      << "\" at "<< pos.x << "," << pos.y
                      << " left font \"Helvetica,8\" textcolor rgb \"blue\" front  point pt 4 ps 0.3 lc rgb \"blue\" offset 0,0"
                      << std::endl;
            }
          else if (mmdev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << mmdev->GetCellId ()
                      << "\" at "<< pos.x << "," << pos.y
                      << " left font \"Helvetica,8\" textcolor rgb \"red\" front  point pt 4 ps 0.3 lc rgb \"red\" offset 0,0"
                      << std::endl;
            } 
         else if (mmIabdev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << mmIabdev->GetCellId ()
                      << "\" at "<< pos.x << "," << pos.y
                      << " left font \"Helvetica,8\" textcolor rgb \"red\" front  point pt 4 ps 0.3 lc rgb \"red\" offset 0,0"
                      << std::endl;
            } 
        }
    }
}

/*
 * Main function.
 */
int
main (int argc, char *argv[])
{
  /*
   * Setup logging
   */	
  LogComponentEnableAll (LOG_PREFIX_TIME);    // Prefix all trace prints with simulation time.
  LogComponentEnableAll (LOG_PREFIX_FUNC);    // Prefix all trace prints with function.
  LogComponentEnableAll (LOG_PREFIX_NODE);    // Prefix all trace prints with simulation node.
  // LogComponentEnable("EpcEnbApplication", LOG_LEVEL_LOGIC);
  LogComponentEnable("EpcIabApplication", LOG_LEVEL_LOGIC);    // Enable EpcIabApplication logging component with LOG_LOGIC (control flow tracing with functions) and above.
  // LogComponentEnable("EpcSgwPgwApplication", LOG_LEVEL_LOGIC);
  // LogComponentEnable("EpcMmeApplication", LOG_LEVEL_LOGIC);
  // LogComponentEnable("EpcUeNas", LOG_LEVEL_LOGIC);

  // LogComponentEnable("LteEnbRrc", LOG_LEVEL_INFO);
  // LogComponentEnable("LteUeRrc", LOG_LEVEL_INFO);
  LogComponentEnable("MmWaveHelper", LOG_LEVEL_LOGIC);
  LogComponentEnable("MmWavePointToPointEpcHelper", LOG_LEVEL_LOGIC);
  //LogComponentEnable("EpcS1ap", LOG_LEVEL_LOGIC);
  // LogComponentEnable("EpcTftClassifier", LOG_LEVEL_LOGIC);
  // LogComponentEnable("EpcGtpuHeader", LOG_LEVEL_INFO);
  // LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  // LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
  LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
  LogComponentEnable("MmWaveIabNetDevice", LOG_LEVEL_INFO);

  
  CommandLine cmd;
  unsigned run = 0;
  bool rlcAm = true;                // rlc is in acknowledge mode
  uint32_t numRelays = 3;           // # of IAB nodes
  uint32_t rlcBufSize = 1000;        // mega-bits, Mb
  uint32_t interPacketInterval = 20000; // micro-second, us
  cmd.AddValue("run", "run for RNG (for generating different deterministic sequences for different drops)", run);
  cmd.AddValue("am", "RLC AM if true", rlcAm);
  cmd.AddValue("numRelay", "Number of relays", numRelays);
  cmd.AddValue("rlcBufSize", "RLC buffer size [MB]", rlcBufSize);
  cmd.AddValue("intPck", "interPacketInterval [us]", interPacketInterval);
  cmd.Parse(argc, argv);

  //   if(rlcAm)
  // {
  //  LogComponentEnable("LteRlcAm", LOG_LEVEL_LOGIC); 
  // }
  // else
  // {
  // LogComponentEnable("MmWaveFlexTtiMacScheduler", LOG_LEVEL_DEBUG);
  // // LogComponentEnable("MmWaveSpectrumPhy", LOG_LEVEL_INFO);
  // LogComponentEnable("MmWaveEnbPhy", LOG_LEVEL_DEBUG);
  // LogComponentEnable("MmWaveUeMac", LOG_LEVEL_DEBUG);
  // LogComponentEnable("MmWaveEnbMac", LOG_LEVEL_DEBUG);
  // }

  Config::SetDefault ("ns3::MmWavePhyMacCommon::UlSchedDelay", UintegerValue(1));
  Config::SetDefault ("ns3::LteRlcAm::MaxTxBufferSize", UintegerValue (rlcBufSize * 1024 * 1024));
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (rlcBufSize * 1024 * 1024));
  Config::SetDefault ("ns3::LteRlcAm::PollRetransmitTimer", TimeValue(MilliSeconds(1.0)));
  Config::SetDefault ("ns3::LteRlcAm::ReorderingTimer", TimeValue(MilliSeconds(2.0)));
  Config::SetDefault ("ns3::LteRlcAm::StatusProhibitTimer", TimeValue(MicroSeconds(500)));
  Config::SetDefault ("ns3::LteRlcAm::ReportBufferStatusTimer", TimeValue(MicroSeconds(500)));
  Config::SetDefault ("ns3::LteRlcUm::ReportBufferStatusTimer", TimeValue(MicroSeconds(500)));
  Config::SetDefault ("ns3::MmWaveHelper::RlcAmEnabled", BooleanValue(rlcAm));

  Config::SetDefault ("ns3::MmWaveFlexTtiMacScheduler::CqiTimerThreshold", UintegerValue(100));

  Config::SetDefault ("ns3::MmWave3gppPropagationLossModel::Scenario", StringValue("UMi-StreetCanyon"));


	RngSeedManager::SetSeed (1);
	RngSeedManager::SetRun (run);

  Config::SetDefault ("ns3::MmWavePhyMacCommon::SymbolsPerSubframe", UintegerValue(24));
  Config::SetDefault ("ns3::MmWavePhyMacCommon::SubframePeriod", DoubleValue(100));
  Config::SetDefault ("ns3::MmWavePhyMacCommon::SymbolPeriod", DoubleValue(100/24));

  Ptr<MmWaveHelper> mmwaveHelper = CreateObject<MmWaveHelper> ();
  mmwaveHelper->SetAttribute ("PathlossModel", StringValue ("ns3::MmWave3gppBuildingsPropagationLossModel"));
  // MmWavePointToPointEpcHelper is used to create the core network
  Ptr<MmWavePointToPointEpcHelper>  epcHelper = CreateObject<MmWavePointToPointEpcHelper> ();
  mmwaveHelper->SetEpcHelper (epcHelper);
  mmwaveHelper->Initialize();

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();

  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  // Get the pointer to the pgw node
  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);  // pgw and remoteHost are connected via p2p channel
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  // Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  // place buildings
  int numBuildingsRow = 4;
  int numBuildingsColumn = 4;
  double streetWidth = 10; // m
  double buildingWidthX = 70; // m
  double buildingWidthY = 70; // m
  double buildingHeight = 10; // m
  std::vector< Ptr<Building> > buildingVector; // in case you need to access the buildings later

  for (int rowIndex = 0; rowIndex < numBuildingsRow; ++rowIndex)
  {
    double minYBuilding = rowIndex*(buildingWidthY + streetWidth);
    for (int colIndex = 0; colIndex < numBuildingsColumn; ++colIndex)
    {
      double minXBuilding = colIndex*(buildingWidthX + streetWidth);
      Ptr <Building> building;
      building = Create<Building> ();
      building->SetBoundaries (Box( minXBuilding, minXBuilding + buildingWidthX,
                                    minYBuilding, minYBuilding + buildingWidthY,
                                    0.0, buildingHeight));
      building->SetNRoomsX(1);
      building->SetNRoomsY(1);
      building->SetNFloors(1);

      buildingVector.push_back(building);
      Box buildingBoxForLog = building->GetBoundaries();
      NS_LOG_INFO("Created building between coordinates (" 
        << buildingBoxForLog.xMin << ", " << buildingBoxForLog.yMin << "), ("
        << buildingBoxForLog.xMax << ", " << buildingBoxForLog.yMin << "), ("
        << buildingBoxForLog.xMin << ", " << buildingBoxForLog.yMax << "), ("
        << buildingBoxForLog.xMax << ", " << buildingBoxForLog.yMax << ") "
        << "with height " << buildingBoxForLog.zMax - buildingBoxForLog.zMin << " m");
    }
  }

  /*
   *  The original two hop (single Iab node) positioning of the nodes in the topology:
   *  
   *  Ue2    Iab4    Ue4
   *
   *  Iab2    gnb    Iab3
   *
   *  Ue1    Iab1    Ue3
   */
  double xMax = numBuildingsRow*(buildingWidthX + streetWidth) - streetWidth;
  double yMax = numBuildingsColumn*(buildingWidthY + streetWidth) - streetWidth;
  double totalArea = xMax * yMax;

  double gnbHeight = buildingHeight + 10;

  double xWired = numBuildingsRow*(buildingWidthX+streetWidth)/2 - streetWidth/2;
  double yWired = numBuildingsColumn*(buildingWidthY+streetWidth)/2 - streetWidth/2;

  double xUe1 = numBuildingsRow*(buildingWidthX+streetWidth)/4 - streetWidth/2;
  double xUe2 = 3*numBuildingsRow*(buildingWidthX+streetWidth)/4 - streetWidth/2;
  double yUe1 = numBuildingsColumn*(buildingWidthY+streetWidth)/4 - streetWidth/2 ;
  double yUe2 = 3*numBuildingsColumn*(buildingWidthY+streetWidth)/4 - streetWidth/2;

  Vector posUe1 = Vector(xUe1, yUe1, gnbHeight);
  Vector posUe2 = Vector(xUe1, yUe2, gnbHeight);
  Vector posUe3 = Vector(xUe2, yUe1, gnbHeight);
  Vector posUe4 = Vector(xUe2, yUe2, gnbHeight);
  Vector posWired = Vector(xWired, yWired, gnbHeight);
  Vector posIab1 = Vector(xWired, yUe1, gnbHeight);
  Vector posIab2 = Vector(xUe1, yWired, gnbHeight);
  Vector posIab3 = Vector(xUe2, yWired, gnbHeight);
  Vector posIab4 = Vector(xWired, yUe2, gnbHeight);

  /*
   *  The three hop scenario
   *
   *           gnb
   *
   *  Iab2     Iab1
   *
   *  Ue1     
   */
  if (numRelays == 2) {
      posWired = Vector(xWired, yUe2, gnbHeight);
      posIab1 = Vector(xWired, yWired, gnbHeight);
      posIab4 = Vector(xWired, yUe1, gnbHeight);

  
  }

  /*
   *  The three hop scenario
   *
   *  Iab3     gnb
   *
   *  Iab2     Iab1
   *
   *  Ue1     
   */

  if (numRelays == 3) {
      posWired = Vector(xUe2, yUe2, gnbHeight);
      posIab1 = Vector(xUe2, yWired, gnbHeight);
      posIab2 = Vector(xWired, yWired, gnbHeight);
      posIab3 = Vector(xWired, yUe2, gnbHeight);
      posUe4 = Vector(xUe1, yUe1, gnbHeight);
  }


  NS_LOG_UNCOND("wired " << posWired << 
                " iab1 " << posIab1 <<
                " iab2 " << posIab2 <<
                " iab3 " << posIab3 << 
                " iab4 " << posIab4 << 
                " totalArea " << totalArea
                );

  /*
   *  Generating nodes in the topology:
   *  1 wired gnb as source, 1 Iab as relay, 1 Ue as destination. 
   */
  NodeContainer ueNodes;
  NodeContainer enbNodes;
  NodeContainer iabNodes;
 
  enbNodes.Create(1);
  iabNodes.Create(numRelays);
  ueNodes.Create(1);

  // Install Mobility Model
  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  enbPositionAlloc->Add (posWired);
  MobilityHelper enbmobility;
  enbmobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  enbmobility.SetPositionAllocator(enbPositionAlloc);
  enbmobility.Install (enbNodes);

  if(numRelays > 0)
  { 
    Ptr<ListPositionAllocator> iabPositionAlloc = CreateObject<ListPositionAllocator> ();
    iabPositionAlloc->Add (posIab1);
    iabPositionAlloc->Add (posIab2);
    iabPositionAlloc->Add (posIab3);
    iabPositionAlloc->Add (posIab4);
    MobilityHelper iabmobility;
    iabmobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    iabmobility.SetPositionAllocator (iabPositionAlloc);
    iabmobility.Install (iabNodes);
  }

  MobilityHelper uemobility;
  Ptr<ListPositionAllocator> uePosAlloc = CreateObject<ListPositionAllocator>();
  uePosAlloc->Add (posUe1);  // Change it to Iab1, when test the LoS single hop case without relaying.
  uePosAlloc->Add (posUe2);
  uePosAlloc->Add (posUe3);
  uePosAlloc->Add (posUe4);
  uemobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  uemobility.SetPositionAllocator (uePosAlloc);
  uemobility.Install (ueNodes);
  
  BuildingsHelper::Install (enbNodes);
  if(numRelays > 0)
  { 
    BuildingsHelper::Install (iabNodes);
  }
  BuildingsHelper::Install (ueNodes);
  BuildingsHelper::MakeMobilityModelConsistent ();

  // Install mmWave Devices to the nodes
  NetDeviceContainer enbmmWaveDevs = mmwaveHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer iabmmWaveDevs;
  if(numRelays > 0)
  {
    iabmmWaveDevs = mmwaveHelper->InstallIabDevice (iabNodes);
  }
  NetDeviceContainer uemmWaveDevs = mmwaveHelper->InstallUeDevice (ueNodes);

  /*
   *  Print nodes information to file.
   */
  PrintGnuplottableBuildingListToFile("buildings.txt");// fileName.str ());
  PrintGnuplottableEnbListToFile("enbs.txt");
  PrintGnuplottableUeListToFile("ues.txt");

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (uemmWaveDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  NetDeviceContainer possibleBaseStations(enbmmWaveDevs, iabmmWaveDevs);
  NS_LOG_UNCOND("number of IAB devs " << iabmmWaveDevs.GetN() << " num of possibleBaseStations " 
    << possibleBaseStations.GetN());

  if(numRelays > 0)
  {
    mmwaveHelper->AttachIabToClosestWiredEnb (iabmmWaveDevs, possibleBaseStations);
  }
  mmwaveHelper->AttachToClosestEnbWithDelay (uemmWaveDevs, possibleBaseStations, Seconds(0.3));

  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1234;
  // uint16_t ulPort = 2000;
  // uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;

  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
  {
    // DL UDP
    UdpServerHelper dlPacketSinkHelper (dlPort);
    serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get(u)));

    UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
    dlClient.SetAttribute ("Interval", TimeValue (MicroSeconds(interPacketInterval)));
    dlClient.SetAttribute ("PacketSize", UintegerValue(1400));
    dlClient.SetAttribute ("MaxPackets", UintegerValue(0xFFFFFFFF));
    clientApps.Add (dlClient.Install (remoteHost));

    dlPort++;
  }
  serverApps.Start (Seconds (0.49));
  clientApps.Stop (Seconds (1.2));
  clientApps.Start (Seconds (0.5));

  mmwaveHelper->EnableTraces ();

  Simulator::Stop(Seconds(1.2));
  Simulator::Run();

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/
  Ptr<UdpServer> udpServer = DynamicCast<UdpServer> (serverApps.Get(0));
  uint64_t totalNumPkt = udpServer->GetReceived();
  NS_LOG_UNCOND("Total number of packets received at server " << totalNumPkt );

  Simulator::Destroy();
  return 0;
}

