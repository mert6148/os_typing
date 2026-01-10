#!/usr/bin/env bash
# Check certificate expiry for files in certs/ (pem, cer, pfx)
# Exit with non-zero if any certificate expires within THRESH_DAYS

set -euo pipefail
THRESH_DAYS=${THRESH_DAYS:-30}
FAIL=0
shopt -s nullglob

for f in certs/*.{pem,cer,pfx}; do
  [ -e "$f" ] || continue
  ext="${f##*.}"
  if [ "$ext" = "pfx" ]; then
    # assumes empty passphrase (development PFX)
    openssl pkcs12 -in "$f" -nokeys -passin pass:"" -clcerts -out /tmp/tmpcert.pem 2>/dev/null
    CERTFILE=/tmp/tmpcert.pem
  else
    CERTFILE="$f"
  fi
  if ! openssl x509 -enddate -noout -in "$CERTFILE" >/dev/null 2>&1; then
    echo "Skipping $f: not a valid cert"
    continue
  fi
  enddate=$(openssl x509 -enddate -noout -in "$CERTFILE" | cut -d= -f2)
  endsec=$(date -d "$enddate" +%s)
  now=$(date +%s)
  days=$(( (endsec - now) / 86400 ))
  echo "$f expires in $days days"
  if [ "$days" -lt "$THRESH_DAYS" ]; then
    echo "Certificate $f expires in $days days (< $THRESH_DAYS)!"
    FAIL=1
  fi
done

if [ "$FAIL" -eq 1 ]; then
  echo "One or more certificates are expiring soon."
  exit 2
fi

echo "All certificates are valid for more than $THRESH_DAYS days."