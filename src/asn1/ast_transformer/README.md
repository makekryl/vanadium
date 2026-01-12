## ASN.1 AST to TTCN-3 AST Transformer

This is the second take on backing ASN.1 support into Vanadium

The first one is known as the *transparser* - a handwritten half-baked ASN.1 parser that parses the ASN.1 source text directly into a TTCN-3 AST and skips uninteresting syntax constructs. Such approach turned to be not vitable for implementing ASN.1 X.681-X.683 support (parametrized types in particular), as TTCN-3 does not support parametrized types at its core, so such types cannot be translated into TTCN-3 directly, requiring a kind of preprocessing stage.

This transformer parses ASN.1 modules into their real corresponding syntax trees, and performs intra- and inter-module
semantic analysis to instantiate the parametrized types, alongside with fixing the identifiers that conflict with TTCN-3 keywords
and building the TTCN-3 AST like the previous parser did.

ASN.1 source code is being parsed by the heavily modified parser from *asn1c*.

### Limitations

Currently, it has some limitations like Eclipse Titan does - primarily not for compatibility reasons,
but because of the technical difficulty of working around this within highly parallel Vanadium architecture.

The limitations are related to parametrized types from ASN.1 X.681-X.683: each instantiation of a parametrized type
is treated as a separate type - even if the parameters are the same - making instantiations of the same type incompatible with each other.

Consider the following example:

```asn
TheAsnModule
DEFINITIONS ::=
BEGIN

TestType { FieldType } ::= SEQUENCE {
  val FieldType
}

TestChoice ::= SEQUENCE {
  field1 TestType { BOOLEAN },
  field2 TestType { BOOLEAN }
}

END
```

```ttcn3
module TheTtcnModule {

import from TheAsnModule all;

function f() {
  var TestChoice c;
  c.field1 := c.field2;
  //       ~~
}

}
```

Obviously `field1` and `field2` have the same type, but Titan will emit an error:

```
Type mismatch: a value of type `@TheAsnModule.TestType.TheAsnModule.inst1' was expected instead of `@TheAsnModule.TestType.TheAsnModule.inst2'
```

Vanadium will do the same as it suffers from the same root problem.
