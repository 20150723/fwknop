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
#include "sha.h"

/* Size calculation macros
*/
#define MD_HEX_SIZE(x) x * 2
#define MD_B64_SIZE(x) ((x * 4) / 3) + 1

void md5(unsigned char* out, unsigned char* in, int size);
void md5_hex(char* out, unsigned char* in, int size);
void md5_base64(char* out, unsigned char* in, int size);
void sha1(unsigned char* out, unsigned char* in, int size);
void sha1_hex(char* out, unsigned char* in, int size);
void sha1_base64(char* out, unsigned char* in, int size);
void sha256(unsigned char* out, unsigned char* in, int size);
void sha256_hex(char* out, unsigned char* in, int size);
void sha256_base64(char* out, unsigned char* in, int size);

#endif /* DIGEST_H */

/***EOF***/
