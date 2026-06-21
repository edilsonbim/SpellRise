import { FileBlob, SpreadsheetFile } from "@oai/artifact-tool";

const input = await FileBlob.load("C:/Users/biM/Downloads/goodgamedatatable.xlsx");
const workbook = await SpreadsheetFile.importXlsx(input);

console.log((await workbook.inspect({
  kind: "sheet",
  include: "id,name",
  maxChars: 4000,
})).ndjson);

console.log((await workbook.inspect({
  kind: "match",
  searchTerm: "falloff|exp",
  options: { useRegex: true, maxResults: 100 },
  summary: "locate falloff exp",
  maxChars: 12000,
})).ndjson);

console.log((await workbook.inspect({
  kind: "table",
  tableMaxRows: 40,
  tableMaxCols: 20,
  tableMaxCellChars: 120,
  maxChars: 20000,
})).ndjson);
