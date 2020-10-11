test_mesh = "";
test_anim = "";
function Body()
	local MsgProc = function( msg )
		if ( msg.id == "filedrop" ) then
			local ext = string.upper(msg.paramb);
			if ( ext == "FBX" ) then
				local cmd = string.format( "Assets\\Models\\FbxToBinary %s.%s Assets\\Models\\", msg.parama, msg.paramb );
				engine.SystemCall( cmd );
				test_mesh = mstring.PathGetFilename( msg.parama );
				test_anim = "";
			elseif ( ext == "MESH" ) then
				test_mesh = mstring.PathGetFilename( msg.parama );
				test_anim = "";
			elseif ( ext == "ANIM" ) then
				test_anim = mstring.PathGetFilename( msg.parama );
			end
		end
	end
	local editorstatus = false;
	SubscribeToMsg( MsgProc );
	while true do
		local editor_on = coroutine.yield();
		if ( (editorstatus) and  (not editor_on ) ) then test_mesh = ""; test_anim = ""; end
		editorstatus = editor_on;
	end
end