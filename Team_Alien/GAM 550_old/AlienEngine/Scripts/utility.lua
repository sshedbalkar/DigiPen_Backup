u = {};

----------------------------------------------------------------------------------
u.CreateThread = function( func, param0 )
	local co = coroutine.create( func );
	local result,retval0, retval1 = coroutine.resume( co, param0 );
	if ( result == false ) then
		print( error );
		return nil;
	end
	return co, retval0, retval1;
end
----------------------------------------------------------------------------------
u.CreateThreadLong = function( func, param0, param1, param2, param3 )
	local co = coroutine.create( func );
	local result,error = coroutine.resume( co, param0, param1, param2, param3 );
	if ( result == false ) then
		print( error );
		return nil;
	end
	return co;
end
----------------------------------------------------------------------------------
u.UpdateThread = function( thread, param0, param1, param2 )
	local result, retval0, retval1, retval2 = coroutine.resume( thread, param0, param1, param2 );
	if ( result == false ) then
		print( retval0 );
		return nil;
	end
	return retval0, retval1, retval2;
end