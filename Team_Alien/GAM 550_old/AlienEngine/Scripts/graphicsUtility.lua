gu = {};
gu.render_depth = 100;
gu.render_diffuse = 101;
gu.samplers = Graphics.Samplers();
gu.blend_states = Graphics.BlendStates();
gu.raster_states = Graphics.RasterStates();
gu.depth_states = Graphics.DepthStates();

render_flags = { 
						DEBUG_SHAPE_BOX = 0,
						DEBUG_SHAPE_ARROW = 1,
						RENDER_FLAG_SHADED = 1,
						RENDER_FLAG_HIGHLIGHTED = 2,
					};
local hlsl_macros = {
						NOTHING = "whatever",
					};

for tag,value in pairs( render_flags ) do
	hlsl_macros[tag] = tostring(value);
end
----------------------------------------------------------------------------------
gu.Setup = function( graphics )
	for tag,value in pairs( hlsl_macros ) do
		graphics.AddMacro( tag, value );
	end
	graphics.ApplyMacros();
end

----------------------------------------------------------------------------------
gu.BuildBasicMatrices = function( ortho, maincamera )
	maincamera:ProjectionFrustum( 0.8, blackboard.screenw / blackboard.screenh, 0.1, 1000.0 );
	ortho:Set( 2, 0, 0, -1.0,
		       0, 2, 0, -1.0,
			   0, 0, 0.001,  0.001,
			   0, 0, 0,  1 );
end
----------------------------------------------------------------------------------
gu.CreateThread = function( func, graphics )
	local co = coroutine.create( func );
	local result,retval0, retval1 = coroutine.resume( co, graphics );
	if ( result == false ) then
		print( error );
		return nil;
	end
	return co, retval0, retval1;
end
----------------------------------------------------------------------------------
gu.CreateThreadLong = function( func, param0, param1, param2, param3 )
	local co = coroutine.create( func );
	local result,error = coroutine.resume( co, param0, param1, param2, param3 );
	if ( result == false ) then
		print( error );
		return nil;
	end
	return co;
end
----------------------------------------------------------------------------------
gu.UpdateThread = function( thread, param0, param1, param2 )
	local result, retval0, retval1, retval2 = coroutine.resume( thread, param0, param1, param2 );
	if ( result == false ) then
		print( retval0 );
		return nil;
	end
	return retval0, retval1, retval2;
end
----------------------------------------------------------------------------------
gu.AddModel = function( func, graphics, model_id, models )
	local co = coroutine.create( func );
	coroutine.resume( co, graphics, model_id );
	table.insert( models, co );
	return co;
end
----------------------------------------------------------------------------------
gu.UpdateThreadList = function( threads, param0, param1 )
	for i, thread in pairs( threads ) do
		local result,error = coroutine.resume( thread, param0, param1 );
		if ( result == false ) then
			print( error ); 
			threads[i] = nil;
		end
	end
end
----------------------------------------------------------------------------------
