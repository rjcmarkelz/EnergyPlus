// EnergyPlus, Copyright (c) 1996-2016, The Board of Trustees of the University of Illinois and
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy). All rights
// reserved.
//
// If you have questions about your rights to use or distribute this software, please contact
// Berkeley Lab's Innovation & Partnerships Office at IPO@lbl.gov.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// ( 1 ) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without Lawrence Berkeley National Laboratory's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the
// features, functionality or performance of the source code ("Enhancements") to anyone; however,
// if you choose to make your Enhancements available either publicly, or directly to Lawrence
// Berkeley National Laboratory, without imposing a separate written license agreement for such
// Enhancements, then you hereby grant the following license: a non-exclusive, royalty-free
// perpetual license to install, use, modify, prepare derivative works, incorporate into other
// computer software, distribute, and sublicense such enhancements or derivative works thereof,
// in binary and source code form.

// EnergyPlus::ZoneTempPredictorCorrector Unit Tests

// Google Test Headers
#include <gtest/gtest.h>
#include "Fixtures/EnergyPlusFixture.hh"
#include <ObjexxFCL/Array.functions.hh>
#include <ObjexxFCL/Array2D.hh>
#include <ObjexxFCL/Fmath.hh>

// EnergyPlus Headers
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataHeatBalFanSys.hh>
#include <EnergyPlus/DataGlobals.hh>
#include <EnergyPlus/DataZoneControls.hh>
#include <EnergyPlus/DataZoneEquipment.hh>
#include <EnergyPlus/DataZoneEnergyDemands.hh>
#include <EnergyPlus/DataSizing.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/UtilityRoutines.hh>
#include <EnergyPlus/ZonePlenum.hh>
#include <EnergyPlus/ZoneTempPredictorCorrector.hh>
#include <EnergyPlus/DataLoopNode.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataAirflowNetwork.hh>
#include <EnergyPlus/Psychrometrics.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/DataRoomAirModel.hh>
#include <EnergyPlus/HybridModel.hh>
#include <EnergyPlus/DataPrecisionGlobals.hh>

using namespace EnergyPlus;
using namespace ObjexxFCL;
using namespace EnergyPlus::DataHeatBalance;
using namespace EnergyPlus::DataHeatBalFanSys;
using namespace DataGlobals;
using namespace EnergyPlus::DataZoneControls;
using namespace EnergyPlus::DataZoneEquipment;
using namespace EnergyPlus::DataZoneEnergyDemands;
using namespace EnergyPlus::DataSizing;
using namespace EnergyPlus::HeatBalanceManager;
using namespace EnergyPlus::ZonePlenum;
using namespace EnergyPlus::ZoneTempPredictorCorrector;
using namespace EnergyPlus::DataLoopNode;
using namespace EnergyPlus::DataHVACGlobals;
using namespace EnergyPlus::DataSurfaces;
using namespace EnergyPlus::DataEnvironment;
using namespace EnergyPlus::DataAirflowNetwork;
using namespace EnergyPlus::Psychrometrics;
using namespace EnergyPlus::ScheduleManager;
using namespace EnergyPlus::DataRoomAirModel;
using namespace EnergyPlus::HybridModel;
using namespace EnergyPlus::DataPrecisionGlobals;

TEST_F( EnergyPlusFixture, ZoneTempPredictorCorrector_CorrectZoneAirTempTest )
{

	// ZoneTempPredictorCorrector variable initialization 
	std::vector< int > controlledZoneEquipConfigNums;
	controlledZoneEquipConfigNums.push_back( 1 );
	Zone.allocate( 1 );
	HybridModelZone.allocate( 1 );
	AirModel.allocate( 1 );
	ZTM1.allocate( 1 );
	ZTM2.allocate( 1 );
	ZTM3.allocate( 1 );
	XMAT.allocate( 1 );
	XM2T.allocate( 1 );
	XM3T.allocate( 1 );
	ZTOC.allocate( 1 );
	ZTOC.allocate( 1 );
	ZTMX.allocate( 1 );
	ZTM1MX.allocate( 1 );
	ZTM1.allocate( 1 );
	WZoneTimeMinus1Temp.allocate( 1 );
	WZoneTimeMinus2Temp.allocate( 1 );
	WZoneTimeMinus3Temp.allocate( 1 );
	WZoneTimeMinus1.allocate( 1 );
	WZoneTimeMinus2.allocate( 1 );
	WZoneTimeMinus3.allocate( 1 );
	AIRRAT.allocate( 1 );
	ZoneAirHumRat.allocate( 1 );
	NonAirSystemResponse.allocate( 1 );
	NonAirSystemResponse( 1 ) = 0.0;
	SysDepZoneLoadsLagged.allocate( 1 );
	SysDepZoneLoadsLagged( 1 ) = 0.0;
	AirflowNetworkExchangeData.allocate( 1 );
	Node.allocate( 1 );
	TempTstatAir.allocate( 1 );
	LoadCorrectionFactor.allocate( 1 );
	MAT.allocate( 1 );
	ZT.allocate( 1 );
	PreviousMeasuredZT1.allocate( 1 );
	PreviousMeasuredZT2.allocate( 1 );
	PreviousMeasuredZT3.allocate( 1 );

	// CalcZoneComponentLoadSums variable initialization
	MCPI.allocate( 1 );
	MCPI( 1 ) = 0.0;
	MCPV.allocate( 1 );
	MCPM.allocate( 1 );
	MCPM( 1 ) = 0.0;
	MCPE.allocate( 1 );
	MCPE( 1 ) = 0.0;
	MCPC.allocate( 1 );
	MCPC( 1 ) = 0.0;
	MDotCPOA.allocate( 1 );
	MDotCPOA( 1 ) = 0.0;
	MDotOA.allocate( 1 );
	MDotOA( 1 ) = 0.0;
	MCPTI.allocate( 1 );
	MCPTI( 1 ) = 0.0;
	MCPTV.allocate( 1 );
	MCPTM.allocate( 1 );
	MCPTM( 1 ) = 0.0;
	MCPTE.allocate( 1 );
	MCPTE( 1 ) = 0.0;
	MCPTC.allocate( 1 );
	MCPTC( 1 ) = 0.0;
	SurfaceWindow.allocate( 1 );
	Surface.allocate( 2 );
	HConvIn.allocate( 1 );
	SNLoadHeatRate.allocate( 1 );
	SNLoadCoolRate.allocate( 1 );
	SNLoadHeatEnergy.allocate( 1 );
	SNLoadCoolEnergy.allocate( 1 );
	ZoneAirRelHum.allocate( 1 );
	IsZoneDV.allocate( 1 );
	ZoneDVMixedFlag.allocate( 1 );
	ZnAirRpt.allocate( 1 );
	ZoneEquipConfig.allocate( 1 );
	ZoneEquipConfig( 1 ).ActualZoneNum = 1;
	ZoneIntGain.allocate( 1 );
	ZoneIntGain( 1 ).NumberOfDevices = 0;
	ZoneEqSizing.allocate( 1 );

	// CorrectZoneHumRat variable initialization
	ZoneLatentGain.allocate( 1 );
	ZoneLatentGain( 1 ) = 0.0;
	SumLatentHTRadSys.allocate( 1 );
	SumLatentHTRadSys( 1 ) = 0.0;
	SumHmARaW.allocate( 1 );
	SumConvHTRadSys.allocate( 1 );
	SumConvHTRadSys( 1 ) = 0.0;
	SumConvPool.allocate( 1 );
	SumConvPool( 1 ) = 0.0;
	SumHmARa.allocate( 1 );
	MixingMassFlowXHumRat.allocate( 1 );
	MixingMassFlowZone.allocate( 1 );
	ZoneW1.allocate( 1 );
	ZoneAirHumRatTemp.allocate( 1 );
	WZoneTimeMinus1Temp.allocate( 1 );
	WZoneTimeMinus2Temp.allocate( 1 );
	WZoneTimeMinus3Temp.allocate( 1 );
	SumLatentPool.allocate( 1 );
	SumLatentPool( 1 ) = 0.0;
	OAMFL.allocate( 1 );
	VAMFL.allocate( 1 );
	EAMFL.allocate( 1 );EAMFL.allocate( 1 );
	CTMFL.allocate( 1 );

	// Parameter setup
	NumOfZones = 1;
	CurZoneEqNum = 1;
	NumZoneReturnPlenums = 0;
	NumZoneSupplyPlenums = 0;
	SimulateAirflowNetwork = 0;
	Zone( 1 ).Multiplier = 1;
	Zone( 1 ).SystemZoneNodeNumber = 1;
	Zone( 1 ).SurfaceFirst = 1;
	Zone( 1 ).SurfaceLast = 2;
	Zone( 1 ).Volume = 1061.88;
	ShortenTimeStepSys = false;
	TimeStepZone = 10.0 / 60.0; // Zone timestep in hours
	TimeStepSys = 10.0 / 60.0;
	Real64 ZoneTempChange = 0.0;
	Real64 PriorTimeStep = 10.0 / 60.0;

	// Hybrid modeling trigger
	FlagHybridModel = true;
	WarmupFlag = false;
	DoingSizing = false;
	UseZoneTimeStepHistory = true;
	DayOfYear = 1;

	// Case 1: Hybrid model internal thermal mass

	HybridModelZone( 1 ).InfiltrationCalc = false;
	HybridModelZone( 1 ).InternalThermalMassCalc = true;
	HybridModelZone( 1 ).HybridStartDayOfYear = 1;
	HybridModelZone( 1 ).HybridEndDayOfYear = 2;
	MAT( 1 ) = 0.0;
	PreviousMeasuredZT1( 1 ) = 0.1;
	PreviousMeasuredZT2( 1 ) = 0.2;
	PreviousMeasuredZT3( 1 ) = 0.3;
	Zone( 1 ).OutDryBulbTemp = -5.21;
	ZoneAirHumRat( 1 ) = 0.002083;
	MCPV( 1 ) = 1414.60; // Assign TempDepCoef
	MCPTV( 1 ) = -3335.10; // Assign TempIndCoef
	OutBaroPress = 99166.67;

	CorrectZoneAirTemp(ZoneTempChange, ShortenTimeStepSys, UseZoneTimeStepHistory, PriorTimeStep);
	EXPECT_NEAR( 15.13, Zone( 1 ).ZoneVolCapMultpSensHM, 0.01 );

	// Case 2: Hybrid model infiltration

	HybridModelZone( 1 ).InfiltrationCalc = true;
	HybridModelZone( 1 ).InternalThermalMassCalc = false;
	HybridModelZone( 1 ).HybridStartDayOfYear = 1;
	HybridModelZone( 1 ).HybridEndDayOfYear = 2;
	MAT( 1 ) = 0.0;
	PreviousMeasuredZT1( 1 ) = 0.02;
	PreviousMeasuredZT2( 1 ) = 0.04;
	PreviousMeasuredZT3( 1 ) = 0.06;
	Zone( 1 ).ZoneVolCapMultpSens = 8.0;
	Zone( 1 ).OutDryBulbTemp = -6.71;
	ZoneAirHumRat( 1 ) = 0.002083;
	MCPV( 1 ) = 539.49; // Assign TempDepCoef
	MCPTV( 1 ) = 270.10; // Assign TempIndCoef
	OutBaroPress = 99250;

	CorrectZoneAirTemp(ZoneTempChange, ShortenTimeStepSys, UseZoneTimeStepHistory, PriorTimeStep);
	EXPECT_NEAR( 0.2444, Zone( 1 ).InfilOAAirChangeRateHM, 0.01 );

}