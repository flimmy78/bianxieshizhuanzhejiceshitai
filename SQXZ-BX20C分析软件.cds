<distribution version="13.0.2" name="SQXZ-BX20C分析软件" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{CE1CA7F7-E74F-49C8-9E1D-B1810094A8D3}">
		<general appName="SQXZ-BX20C分析软件" outputLocation="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\cvidistkit.SQXZ-BX20C分析软件" relOutputLocation="cvidistkit.SQXZ-BX20C分析软件" outputLocationWithVars="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.0">
			<arp company="" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="Chinese (Simplified)" showPaths="true" readMe="" license="">
			<dlgstrings welcomeTitle="SQXZ-BX20C分析软件" welcomeText=""/></userinterface>
		<dirs appDirID="101">
			<installDir name="SQXZ-BX20C分析软件" dirID="100" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="SQXZ-BX20C分析软件" dirID="101" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\cvibuild.Desktop\Release\SQXZ-BX20C分析软件.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\Waveform\演示波形.vif" relSourcePath="Waveform\演示波形.vif" relSourceBase="0" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\Portable.uir" relSourcePath="Portable.uir" relSourceBase="1" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="101" projectID="0">
				<fileID>0</fileID></projectOutput>
			<projectDependencies dirID="101" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="SQXZ-BX20C分析软件" targetFileID="0" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<runtimeEngine installToAppDir="false" activeXsup="true" analysis="true" cvirte="true" dotnetsup="true" instrsup="false" lowlevelsup="true" lvrt="false" netvarsup="true" rtutilsup="true">
			<hasSoftDeps/></runtimeEngine><sxsRuntimeEngine>
			<selected>false</selected>
			<doNotAutoSelect>false</doNotAutoSelect></sxsRuntimeEngine>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WINXP_SP0</condition>
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
			<Project000 ProjectID="0" ProjectAbsolutePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\Desktop.prj" ProjectRelativePath="Desktop.prj"/>
			<Project001 ProjectID="1" ProjectAbsolutePath="d:\document\zhangpei\working\便携式转辙机拉力测试仪\working\ZZJ.prj" ProjectRelativePath="ZZJ.prj"/></Projects>
	</msi>
</distribution>
