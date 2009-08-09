/* $Id$
 *****************************************************************************
 *
 * File:    digest.h
 *
 * Author:  Damien S. Stuart
 *
 * Purpose: Header for the fwknop digest.c.
 *
 *  License (GNU Public License):
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *     USA
 *
 *****************************************************************************
*/
#ifndef DIGEST_H
#define DIGEST_H 1

#include "fko_common.h"

#include "md5.h"
#include "sha1.h"
#include "sha2.h"

/* Size calculation macros
*/
#define MD_HEX_SIZE(x) x * 2

/* Predefined base64 encoded digest sizes.
*/
#define MD5_B64_LENGTH      22
#define SHA1_B64_LENGTH     27
#define SHA256_B64_LENGTH   43
#define SHA384_B64_LENGTH   64
#define SHA512_B64_LENGTH   86

void md5(unsigned char* out, unsigned char* in, size_t size);
void md5_hex(char* out, unsigned char* in, size_t size);
void md5_base64(char* out, unsigned char* in, size_t size);
void sha1(unsigned char* out, unsigned char* in, size_t size);
void sha1_hex(char* out, unsigned char* in, size_t size);
void sha1_base64(char* out, unsigned char* in, size_t size);
void sha256(unsigned char* out, unsigned char* in, size_t size);
void sha256_hex(char* out, unsigned char* in, size_t size);
void sha256_base64(char* out, unsigned char* in, size_t size);
void sha384(unsigned char* out, unsigned char* in, size_t size);
void sha384_hex(char* out, unsigned char* in, size_t size);
void sha384_base64(char* out, unsigned char* in, size_t size);
void sha512(unsigned char* out, unsigned char* in, size_t size);
void sha512_hex(char* out, unsigned char* in, size_t size);
void sha512_base64(char* out, unsigned char* in, size_t size);

#endif /* DIGEST_H */

/***EOF***/
