function protect_table (tbl)
  return setmetatable ({}, 
    {
    __index = tbl,  -- read access gets original table item
    __newindex = function (t, n, v)
       error ("attempting to change constant " .. 
             tostring (n) .. " to " .. tostring (v), 2)
      end -- __newindex 
    })

end -- function protect_table

C = 
{
------##### virtual keys, add if you need more #####------
	VK_LBUTTON = 0x01,	VK_RBUTTON = 0x02, 
	VK_BACK = 0x08,										-- backspace
	VK_TAB = 0x09, 
	VK_RETURN = 0x0D,
	VK_SHIFT = 0x10,	VK_CONTROL = 0x11, 
	VK_ESCAPE = 0x1B, 
	VK_SPACE = 0x20,									-- spacebar
	VK_PRIOR = 0x21,									-- page up
	VK_NEXT = 0x22,										-- page down
	VK_END = 0x23, VK_HOME = 0x24,
	VK_LEFT = 0x25,	VK_UP = 0x26,	VK_RIGHT = 0x27,	VK_DOWN = 0x28,
	VK_DELETE = 0x2E,
	VK_0 = 0x30,	VK_1 = 0x31,	VK_2 = 0x32,	VK_3 = 0x33,	VK_4 = 0x34,
	VK_5 = 0x35,	VK_6 = 0x36,	VK_7 = 0x37,	VK_8 = 0x38,	VK_9 = 0x39,

	VK_A = 0x41,	VK_B = 0x42,	VK_C = 0x43,	VK_D = 0x44,	VK_E = 0x45,	VK_F = 0x46,
	VK_G = 0x47,	VK_H = 0x48,	VK_I = 0x49,	VK_J = 0x4A,	VK_K = 0x4B,	VK_L = 0x4C,
	VK_M = 0x4D,	VK_N = 0x4E,	VK_O = 0x4F,	VK_P = 0x50,	VK_Q = 0x51,	VK_R = 0x52,
	VK_S = 0x53,	VK_T = 0x54,	VK_U = 0x55,	VK_V = 0x56,	VK_W = 0x57,	VK_X = 0x58,
	VK_Y = 0x59,	VK_Z = 0x5A,

	--graphics constants
	POINTLIST = 1, LINELIST = 2, LINESTRIP = 3, TRIANGLELIST = 4, TRIANGLESTRIP = 5, 
	PATCH1 = 33, PATCH2 = 34, PATCH3 = 35, PATCH4 = 36,
}

C = protect_table( C );