function Body( object )
	----------------------------------------------------------------------------------------- pointers
	local camera = pointer.Camera();
	----------------------------------------------------------------------------------------- inits
	levelinfo = {};
	levelinfo.camera = camera;
	----------------------------------------------------------------------------------------- get pointers
	object:Camera( camera );
	
	while true do
		coroutine.yield();
	end

end