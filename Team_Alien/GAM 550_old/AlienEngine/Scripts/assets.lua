textures = {};
meshes = {};
effects = {};
animations = {};



--------------------------------------------- test level assets -----------------------------------------------
function LoadAssets( graphics )
	--------------------------------------------- textures --------------------------------------------
	--textures["font_tnr"] = graphics.SRVFromFile( "Assets\\Fonts\\TimesNewRoman.png" );
	textures["cloud"] = graphics.SRVFromFile( "Assets\\textures\\cloud.png" );
	textures["perlin"] = graphics.SRVFromFile( "Assets\\textures\\perlinNoise.png" );
	textures["shade"] = graphics.SRVFromFile( "Assets\\textures\\shade.png" );
	---------------------------------------------- meshes ---------------------------------------------
	table.insert( meshes, "tad" );
	table.insert( meshes, "ufo" );
	----------------------------------------------- anims ---------------------------------------------
	animations["tadanim"] = graphics.LoadAnim( "tad000" );
	---------------------------------------------- effects --------------------------------------------
	effects["smodel"]			= graphics.LoadEffect( "shaders\\Model.fx", "f3Vf3Vf3Vf3Vf2Vi1Im1I", C.TRIANGLELIST, "VSStatic", "PS", "GS" );
	effects["amodel"]			= graphics.LoadEffect( "shaders\\Model.fx", "f3Vf3Vf3Vf3Vf2Vi4Vf4Vi1Im1I", C.TRIANGLELIST, "VSAnimated", "PS", "GS" );
	effects["smodeldepth"]		= graphics.LoadEffect( "shaders\\modelDepth.fx", "f3Vi1Im1I", C.TRIANGLELIST, "VSStatic", "PS" );
	effects["amodeldepth"]		= graphics.LoadEffect( "shaders\\modelDepth.fx", "f3Vi4Vf4Vi1Im1I", C.TRIANGLELIST, "VSAnimated", "PS" );
	effects["debug"]			= graphics.LoadEffect( "shaders\\debugcube.fx", "m1Vf3Vf3Vi1Vi1V", C.POINTLIST, "VS", "PS", "GS" );
	effects["debug2dsolid"]		= graphics.LoadEffect( "shaders\\debug2d.fx", "f3Vf3Vf4Vi1V", C.POINTLIST, "VS", "PS", "GSSolid" );
	effects["debug2dframe"]		= graphics.LoadEffect( "shaders\\debug2d.fx", "f3Vf3Vf4Vi1V", C.POINTLIST, "VS", "PS", "GSFrame" );
	effects["heightmappedmesh"]	= graphics.LoadEffect( "shaders\\heightmappedMesh.fx", "f2V", C.TRIANGLELIST, "VS", "PS" );
	effects["sprite"]			= graphics.LoadEffect( "shaders\\sprite.fx", "f2Vf2Vf4V", C.POINTLIST, "VS", "PS", "GS" );
	effects["sky"]				= graphics.LoadEffect( "shaders\\sky.fx", "f3Vf2V", C.TRIANGLELIST, "VS", "PS" );
	
	effects["deferred"]			= graphics.LoadEffect( "shaders\\deferredPass.fx", "f2V", C.POINTLIST, "VS", "PS", "GS" );
	effects["luma"]				= graphics.LoadEffect( "shaders\\hdr.fx", "f2V", C.POINTLIST, "VS", "PSLuma", "GS" );
	effects["adaptation"]		= graphics.LoadEffect( "shaders\\hdr.fx", "f2V", C.POINTLIST, "VS", "PSAdaptation", "GS" );
	effects["highlight"]		= graphics.LoadEffect( "shaders\\hdr.fx", "f2V", C.POINTLIST, "VS", "PSHighlight", "GS" );
	effects["bloom"]			= graphics.LoadEffect( "shaders\\hdr.fx", "f2V", C.POINTLIST, "VS", "PSBloom", "GS" );
	effects["tonemap"]			= graphics.LoadEffect( "shaders\\hdr.fx", "f2V", C.POINTLIST, "VS", "PSToneMap", "GS" );
	effects["fxaa"]				= graphics.LoadEffect( "shaders\\fxaa.fx", "f2V", C.POINTLIST, "VS", "PS", "GS" );

	effects["testso"]			= graphics.LoadEffectSO( "shaders\\testSO.fx", "f3V", C.POINTLIST, "VS", "", "GS" );
	effects["testculling"]		= graphics.LoadEffect( "shaders\\testD.fx", "f3V", C.POINTLIST, "VS", "PS", "GS" );
end

function LoadLevel1Assets( graphics )
end
