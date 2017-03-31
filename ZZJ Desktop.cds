<distribution version="13.0.2" name="SQXZ-BX20C转辙机拉力测试仪PC软件" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{35400CEB-17CC-4A4C-8470-C09812DBCECA}">
		<general appName="SQXZ-BX20C分析软件" outputLocation="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\cvibuild.SQXZ-BX20C" relOutputLocation="cvibuild.SQXZ-BX20C" outputLocationWithVars="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\cvibuild.SQXZ-BX20C" relOutputLocationWithVars="cvibuild.SQXZ-BX20C" upgradeBehavior="1" autoIncrement="true" version="1.0.10">
			<arp company="陕西齐力铁路电气限公司" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="SQXZ-BX20C型转辙机拉力测试仪PC软件" subject="" keyWords="" comments="" author="陕西齐力铁路电气限公司"/></general>
		<userinterface language="Chinese (Simplified)" showPaths="true" readMe="" license="">
			<dlgstrings welcomeTitle="SQXZ-BX20C分析软件" welcomeText=""/></userinterface>
		<dirs appDirID="101">
			<installDir name="SQXZ-BX20C转辙机拉力测试仪分析软件" dirID="100" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="SQXZ-BX20C转辙机拉力测试仪分析软件" dirID="101" parentID="2" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\Portable.uir" relSourcePath="Portable.uir" relSourceBase="0" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\SQXZ-BX20C型转辙机拉力测试仪使用说明书.doc" relSourcePath="SQXZ-BX20C型转辙机拉力测试仪使用说明书.doc" relSourceBase="1" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\Waveform\演示波形.vif" relSourcePath="Waveform\演示波形.vif" relSourceBase="1" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="3" sourcePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\cvibuild.Desktop\Release\SQXZ-BX20C分析软件.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="101" projectID="1">
				<fileID>3</fileID></projectOutput>
			<projectDependencies dirID="101" projectID="1"/></fileGroups>
		<shortcuts>
			<shortcut name="SQXZ-BX20C分析软件" targetFileID="3" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/>
			<shortcut name="SQXZ-BX20C使用说明" targetFileID="1" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products>
			<product name="NI LabWindows/CVI Shared Run-Time Engine" UC="{80D3D303-75B9-4607-9312-E5FC68E5BFD2}" productID="{D3C549FD-7DA5-440B-A1BC-DD92C898949A}" path="C:\ProgramData\National Instruments\MDF\ProductCache\" flavorID="_full_" flavorName="Full" verRestr="false" coreVer="">
				<cutSoftDeps>
					<softDep name=".NET Support" UC="{0DDB211A-941B-4125-9518-E81E10409F2E}" depKey="CVI_DotNet.CVI1320.RTE"/>
					<softDep name="ActiveX Container Support" UC="{1038A887-23E1-4289-B0BD-0C4B83C6BA21}" depKey="ActiveX_Container.CVI1320.RTE"/>
					<softDep name="Dev (Patch 1) Support" UC="{C6F76062-CDCB-4931-8580-012AF2152602}" depKey="CVIRTEUpdated.CVI1320.RTE"/>
					<softDep name="Low-Level Driver Support" UC="{20931852-6AA9-4918-926B-130D07A4CF8B}" depKey="CVI_LowLevelDriver.CVI1320.RTE"/>
					<softDep name="Network Streams Support" UC="{40A5AD7F-4BAF-4A5C-8B56-426B84F75C05}" depKey="CVI_NetworkStreams.CVI1320.RTE"/>
					<softDep name="Network Variable Support" UC="{15CE39FE-1354-484D-B8CA-459077449FB3}" depKey="CVI_NetworkVariable.CVI1320.RTE"/>
					<softDep name="Real-Time Utility Support" UC="{DA6BBF6F-0910-4860-AD46-F14DADF645E9}" depKey="Remote_Configuration.CVI1320.RTE"/>
					<softDep name="TDMS Support" UC="{5A8AF88D-486D-4E30-A7A5-8D8A039BBEBF}" depKey="CVI_Tdms.CVI1320.RTE"/>
					<softDep name="Visual C++ 2008 Redistributable" UC="{BD1E864A-2ACC-4F7E-9A7D-977C39952A52}" depKey="VC2008MSMs_Redist.CVI1320.RTE"/>
					<softDep name="Visual C++ 2008 Runtime" UC="{FDA3F8BB-BAA9-45D7-8DC7-22E1F5C76315}" depKey="VC2008MSMs.CVI1320.RTE"/>
					<softDep name="Visual C++ 2010 Redistributable" UC="{3109870C-C165-4896-AC99-AFDD3CA3304A}" depKey="VC2010MSMs_Redist.CVI1320.RTE"/></cutSoftDeps></product></products>
		<runtimeEngine installToAppDir="false" activeXsup="false" analysis="true" cvirte="true" dotnetsup="false" instrsup="true" lowlevelsup="false" lvrt="true" netvarsup="false" rtutilsup="false">
			<hasSoftDeps/></runtimeEngine><sxsRuntimeEngine>
			<selected>false</selected>
			<doNotAutoSelect>false</doNotAutoSelect></sxsRuntimeEngine>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WINXP</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<Projects NumProjects="2">
			<Project000 ProjectID="0" ProjectAbsolutePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\ZZJ.prj" ProjectRelativePath="ZZJ.prj"/>
			<Project001 ProjectID="1" ProjectAbsolutePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\Desktop.prj" ProjectRelativePath="Desktop.prj"/></Projects>
		<buildData progressBarRate="10.256726081182777">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.137965416015625</ProductsAdded>
				<DPConfigured>0.449976065429681</DPConfigured>
				<DPMergeModulesAdded>1.603369600585939</DPMergeModulesAdded>
				<DPClosed>2.475580534179692</DPClosed>
				<DistributionsCopied>2.612732342773430</DistributionsCopied>
				<End>9.749699778320323</End></progressTimes></buildData>
	</msi>
</distribution>
