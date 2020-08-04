// This file is part of Notepad++ project
// Copyright (C)2003-2017 Don HO <don.h@free.fr>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// Note that the GPL places important restrictions on "derived works", yet
// it does not provide a detailed definition of that term.  To avoid
// misunderstandings, we consider an application to constitute a
// "derivative work" for the purpose of this license if it does any of the
// following:
// 1. Integrates source code from Notepad++.
// 2. Integrates/includes/aggregates Notepad++ into a proprietary executable
//    installer, such as those produced by InstallShield.
// 3. Links to a library or executes a program that does any of the above.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#pragma once


//#define VerifySignedLibrary_DISABLE_REVOCATION_CHECK "Dont check certificat revocation"

/*
* Verifies an Authenticde DLL signature and ownership
*
* Parameters:
*  @param filepath        path to the DLL file to examine
*  @param cert_display_name if specified, the signing certificate display name to compare to. Ignored if set to "", (weak comparison)
*  @param cert_subject    if specified, the full signing certificate subject name. Ignored if set to "" (strong comparison)
*  @param cert_key_id_hex if specified, the signing certificate key id (fingerprint), Ignored if set to "" (very strong comparison)
*
* @return true if the verification was positive, false if it was negative of encountered some error
*
* Dependencies:
*  This function uses 3 APIs: WinTrust, CryptoAPI, SENS API
*  It requires to link on : wintrust.lib, crypt32.lib (or crypt64.lib depending on the compilation target) and sensapi.lib
*  Those functions are available on Windows starting with Windows-XP
*
* Limitations:
*  Certificate revocation checking requires an access to Internet.
*  The functions checks for connectivity and will disable revocation checking if the machine is offline or if Microsoft
*  connectivity checking site is not reachable (supposely implying we are on an airgapped network).
*  Depending on Windows version, this test will be instantaneous (Windows 8 and up) or may take a few seconds.
*  This behaviour can be disabled by setting a define at compilation time.
*  If macro VerifySignedLibrary_DISABLE_REVOCATION_CHECK is defined, the revocation
*  state of the certificates will *not* be checked.
*
*/

#include <string>

bool VerifySignedLibrary(const std::wstring& filepath,
                         const std::wstring& key_id_hex,
                         const std::wstring& cert_subject,
                         const std::wstring& display_name,
                         bool doCheckRevocation,
                         bool doCheckChainOfTrust);
