Parses ASN.1 source code directly to TTCN-3 AST
Uninterested language constructs are skipped

This "transparser" had to be dropped due to fundamental limitations that block ASN.1 X.681-X.683 support:
parametrized types needs to be statically instantiated as TTCN-3 - the target language - does not support generics.
So, ASN.1 source code has to be parsed into its own AST and then preprocessed before the translation into TTCN-3 types.

Preserved as for reference if there will be need to back another foreign language support into Vanadium

IDK about compiler though, during the compiler development I'd need to have the actual ASN.1 AST anyway to build proper codecs
