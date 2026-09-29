# NeoWPF — How to document and how to NOT document headers

## General Rules

* Document public classes, structs, enums, functions, properties, events, and typedefs.
* Keep `@summary` to a single sentence whenever possible.
* Use `@details` only when additional explanation is necessary.
* Omit unused tags.
* Do not document implementation details or algorithms.

## Available Tags

| Tag             | Description                                     |
| --------------- | ----------------------------------------------- |
| `@summary`      | Brief description of the declaration.           |
| `@details`      | Additional information about behavior or usage. |
| `@param <name>` | Describes a parameter. One tag per parameter.   |
| `@returns`      | Describes the return value.                     |
| `@example`      | Example usage.                                  |
| `@internal`     | Internal methods that aren't public API's       |

