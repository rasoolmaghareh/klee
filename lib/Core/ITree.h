/*
 * ITree.h
 *
 *  Created on: Oct 15, 2015
 *      Author: felicia
 */

#ifndef ITREE_H_
#define ITREE_H_

#include <klee/Expr.h>
#include "klee/Config/Version.h"
#include "klee/ExecutionState.h"

#include "Dependency.h"

#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace klee {
class ExecutionState;

class PathCondition;

class SubsumptionTableEntry;

/// Global variable denoting whether interpolation is enabled or otherwise
struct InterpolationOption {
  static bool interpolation;
};

/// Storage of search tree for displaying
class SearchTree {

  /// @brief counter for the next visited node id
  static unsigned long nextNodeId;

  /// @brief Global search tree instance
  static SearchTree *instance;

  /// Encapsulates functionality of expression builder
  class PrettyExpressionBuilder {

    std::string bvOne(unsigned width);
    std::string bvZero(unsigned width);
    std::string bvMinusOne(unsigned width);
    std::string bvConst32(unsigned width, uint32_t value);
    std::string bvConst64(unsigned width, uint64_t value);
    std::string bvZExtConst(unsigned width, uint64_t value);
    std::string bvSExtConst(unsigned width, uint64_t value);

    std::string bvBoolExtract(std::string expr, int bit);
    std::string bvExtract(std::string expr, unsigned top, unsigned bottom);
    std::string eqExpr(std::string a, std::string b);

    // logical left and right shift (not arithmetic)
    std::string bvLeftShift(std::string expr, unsigned shift);
    std::string bvRightShift(std::string expr, unsigned shift);
    std::string bvVarLeftShift(std::string expr, std::string shift);
    std::string bvVarRightShift(std::string expr, std::string shift);
    std::string bvVarArithRightShift(std::string expr, std::string shift);

    // Some STP-style bitvector arithmetic
    std::string bvMinusExpr(unsigned width, std::string minuend,
                            std::string subtrahend);
    std::string bvPlusExpr(unsigned width, std::string augend,
                           std::string addend);
    std::string bvMultExpr(unsigned width, std::string multiplacand,
                           std::string multiplier);
    std::string bvDivExpr(unsigned width, std::string dividend,
                          std::string divisor);
    std::string sbvDivExpr(unsigned width, std::string dividend,
                           std::string divisor);
    std::string bvModExpr(unsigned width, std::string dividend,
                          std::string divisor);
    std::string sbvModExpr(unsigned width, std::string dividend,
                           std::string divisor);
    std::string notExpr(std::string expr);
    std::string bvNotExpr(std::string expr);
    std::string andExpr(std::string lhs, std::string rhs);
    std::string bvAndExpr(std::string lhs, std::string rhs);
    std::string orExpr(std::string lhs, std::string rhs);
    std::string bvOrExpr(std::string lhs, std::string rhs);
    std::string iffExpr(std::string lhs, std::string rhs);
    std::string bvXorExpr(std::string lhs, std::string rhs);
    std::string bvSignExtend(std::string src, unsigned width);

    // Some STP-style array domain interface
    std::string writeExpr(std::string array, std::string index,
                          std::string value);
    std::string readExpr(std::string array, std::string index);

    // ITE-expression constructor
    std::string iteExpr(std::string condition, std::string whenTrue,
                        std::string whenFalse);

    // Bitvector length
    int getBVLength(std::string expr);

    // Bitvector comparison
    std::string bvLtExpr(std::string lhs, std::string rhs);
    std::string bvLeExpr(std::string lhs, std::string rhs);
    std::string sbvLtExpr(std::string lhs, std::string rhs);
    std::string sbvLeExpr(std::string lhs, std::string rhs);

    std::string existsExpr(std::string body);

    std::string constructAShrByConstant(std::string expr, unsigned shift,
                                        std::string isSigned);
    std::string constructMulByConstant(std::string expr, unsigned width,
                                       uint64_t x);
    std::string constructUDivByConstant(std::string expr_n, unsigned width,
                                        uint64_t d);
    std::string constructSDivByConstant(std::string expr_n, unsigned width,
                                        uint64_t d);

    std::string getInitialArray(const Array *os);
    std::string getArrayForUpdate(const Array *root, const UpdateNode *un);

    std::string constructActual(ref<Expr> e, int *width_out);
    std::string construct(ref<Expr> e, int *width_out);

    std::string buildVar(const char *name, unsigned width);
    std::string buildArray(const char *name, unsigned indexWidth,
                           unsigned valueWidth);

    std::string getTrue();
    std::string getFalse();
    std::string getTempVar(Expr::Width w);
    std::string getInitialRead(const Array *os, unsigned index);

  public:
    PrettyExpressionBuilder();

    ~PrettyExpressionBuilder();

    std::string construct(ref<Expr> e);
  };

  /// Node information
  class Node {
    friend class SearchTree;

    /// @brief Interpolation tree node id
    uintptr_t iTreeNodeId;

    /// @brief The node id, also the order in which it is traversed
    unsigned long nodeId;

    /// @brief False and true children of this node
    SearchTree::Node *falseTarget, *trueTarget;

    /// @brief Indicates that node is subsumed
    bool subsumed;

    /// @brief Conditions under which this node is visited from its parent
    std::map<PathCondition *, std::pair<std::string, bool> > pathConditionTable;

    Node(uintptr_t nodeId)
        : iTreeNodeId(nodeId), nodeId(0), falseTarget(0), trueTarget(0),
          subsumed(false) {}

    ~Node() {
      if (falseTarget)
        delete falseTarget;

      if (trueTarget)
        delete trueTarget;

      pathConditionTable.clear();
    }

    static SearchTree::Node *createNode(uintptr_t id) {
      return new SearchTree::Node(id);
    }
  };

  SearchTree::Node *root;
  std::map<ITreeNode *, SearchTree::Node *> itreeNodeMap;
  std::map<SubsumptionTableEntry *, SearchTree::Node *> tableEntryMap;
  std::map<SearchTree::Node *, SearchTree::Node *> subsumptionEdges;
  std::map<PathCondition *, SearchTree::Node *> pathConditionMap;

  static std::string recurseRender(const SearchTree::Node *node);

  std::string render();

  SearchTree(ITreeNode *_root);

  ~SearchTree();

public:
  static void initialize(ITreeNode *root) {
    if (!instance)
      delete instance;
    instance = new SearchTree(root);
  }

  static void deallocate() {
    if (!instance)
      delete instance;
    instance = 0;
  }

  static void addChildren(ITreeNode *parent, ITreeNode *falseChild,
                          ITreeNode *trueChild);

  static void setCurrentNode(ITreeNode *iTreeNode,
                             const uintptr_t programPoint);

  static void markAsSubsumed(ITreeNode *iTreeNode,
                             SubsumptionTableEntry *entry);

  static void addPathCondition(ITreeNode *iTreeNode,
                               PathCondition *pathCondition,
                               ref<Expr> condition);

  static void addTableEntryMapping(ITreeNode *iTreeNode,
                                   SubsumptionTableEntry *entry);

  static void includeInInterpolant(PathCondition *pathCondition);

  /// @brief Save the graph
  static void save(std::string dotFileName);
};

/**/

class PathCondition {
  /// @brief KLEE expression
  ref<Expr> constraint;

  /// @brief KLEE expression with variables (arrays) replaced by their shadows
  ref<Expr> shadowConstraint;

  /// @brief If shadow consraint had been generated: We generate shadow
  /// constraint
  /// on demand only when the constraint is required in an interpolant
  bool shadowed;

  /// @brief The dependency information for the current
  /// interpolation tree node
  Dependency *dependency;

  /// @brief the condition value from which the
  /// constraint was generated
  VersionedValue *condition;

  /// @brief When true, indicates that the constraint should be included
  /// in the interpolant
  bool inInterpolant;

  /// @brief Previous path condition
  PathCondition *tail;

public:
  PathCondition(ref<Expr> &constraint, Dependency *dependency,
                llvm::Value *condition, PathCondition *prev);

  ~PathCondition();

  ref<Expr> car() const;

  PathCondition *cdr() const;

  void includeInInterpolant(AllocationGraph *g);

  bool carInInterpolant() const;

  ref<Expr> packInterpolant(std::vector<const Array *> &replacements);

  void dump();

  void print(llvm::raw_ostream &stream);
};

class PathConditionMarker {
  bool mayBeInInterpolant;

  PathCondition *pathCondition;

public:
  PathConditionMarker(PathCondition *pathCondition);

  ~PathConditionMarker();

  void includeInInterpolant(AllocationGraph *g);

  void mayIncludeInInterpolant();
};

class SubsumptionTableEntry {
  uintptr_t nodeId;

  ref<Expr> interpolant;

  std::map<llvm::Value *, ref<Expr> > singletonStore;

  std::vector<llvm::Value *> singletonStoreKeys;

  std::map<llvm::Value *, std::vector<ref<Expr> > > compositeStore;

  std::vector<llvm::Value *> compositeStoreKeys;

  std::vector<const Array *> existentials;

  static bool hasExistentials(std::vector<const Array *> &existentials,
                              ref<Expr> expr);

  static ref<Expr> createBinaryOfSameKind(ref<Expr> originalExpr,
                                          ref<Expr> newLhs, ref<Expr> newRhs);

  static bool containShadowExpr(ref<Expr> expr, ref<Expr> shadowExpr);

  static ref<Expr> replaceExpr(ref<Expr> originalExpr, ref<Expr> replacedExpr,
                               ref<Expr> withExpr);

  static ref<Expr>
  simplifyInterpolantExpr(std::vector<ref<Expr> > &interpolantPack,
                          ref<Expr> expr);

  static ref<Expr> simplifyEqualityExpr(std::vector<ref<Expr> > &equalityPack,
                                        ref<Expr> expr);

  static ref<Expr> simplifyWithFourierMotzkin(ref<Expr> existsExpr);

  static ref<Expr> simplifyExistsExpr(ref<Expr> existsExpr);

  static ref<Expr> simplifyArithmeticBody(ref<Expr> existsExpr);

  bool empty() {
    return !interpolant.get() && singletonStoreKeys.empty() &&
           compositeStoreKeys.empty();
  }

public:
  SubsumptionTableEntry(ITreeNode *node);

  ~SubsumptionTableEntry();

  bool subsumed(TimingSolver *solver, ExecutionState &state, double timeout);

  void dump() const;

  void print(llvm::raw_ostream &stream) const;
};

class ITree {
  typedef std::vector<ref<Expr> > ExprList;
  typedef ExprList::iterator iterator;
  typedef ExprList::const_iterator const_iterator;

  ITreeNode *currentINode;

  std::vector<SubsumptionTableEntry *> subsumptionTable;

  void printNode(llvm::raw_ostream &stream, ITreeNode *n, std::string edges);

public:
  ITreeNode *root;

  ITree(ExecutionState *_root);

  ~ITree();

  std::vector<SubsumptionTableEntry *> getStore();

  void store(SubsumptionTableEntry *subItem);

  void setCurrentINode(ITreeNode *node, uintptr_t programPoint);

  void remove(ITreeNode *node);

  bool checkCurrentStateSubsumption(TimingSolver *solver, ExecutionState &state,
                                    double timeout);

  void markPathCondition(ExecutionState &state, TimingSolver *solver);

  std::pair<ITreeNode *, ITreeNode *>
  split(ITreeNode *parent, ExecutionState *left, ExecutionState *right);

  void executeAbstractBinaryDependency(llvm::Instruction *i,
                                       ref<Expr> valueExpr, ref<Expr> tExpr,
                                       ref<Expr> fExpr);

  void executeAbstractMemoryDependency(llvm::Instruction *instr,
                                       ref<Expr> value, ref<Expr> address);

  void executeAbstractDependency(llvm::Instruction *instr, ref<Expr> value);

  void print(llvm::raw_ostream &stream);

  void dump();
};

class ITreeNode {
  friend class ITree;

  friend class ExecutionState;

  typedef ref<Expr> expression_type;

  typedef std::pair<expression_type, expression_type> pair_type;

  /// @brief The path condition
  PathCondition *pathCondition;

  /// @brief Abstract stack for value dependencies
  Dependency *dependency;

  ITreeNode *parent, *left, *right;

  uintptr_t nodeId;

  bool isSubsumed;

  /// @brief Graph for displaying as .dot file
  SearchTree *graph;

  void setNodeLocation(uintptr_t programPoint) {
    if (!nodeId)
      nodeId = programPoint;
  }

public:
  uintptr_t getNodeId();

  ref<Expr> getInterpolant(std::vector<const Array *> &replacements) const;

  void addConstraint(ref<Expr> &constraint, llvm::Value *value);

  void split(ExecutionState *leftData, ExecutionState *rightData);

  void dump() const;

  void print(llvm::raw_ostream &stream) const;

  std::map<ref<Expr>, PathConditionMarker *> makeMarkerMap() const;

  static void
  deleteMarkerMap(std::map<ref<Expr>, PathConditionMarker *> &markerMap);

  void executeBinaryDependency(llvm::Instruction *i, ref<Expr> valueExpr,
                               ref<Expr> tExpr, ref<Expr> fExpr);

  void executeAbstractMemoryDependency(llvm::Instruction *instr,
                                       ref<Expr> value, ref<Expr> address);

  void executeAbstractDependency(llvm::Instruction *instr, ref<Expr> value);

  void bindCallArguments(llvm::Instruction *site,
                         std::vector<ref<Expr> > &arguments);

  void popAbstractDependencyFrame(llvm::CallInst *site, llvm::Instruction *inst,
                                  ref<Expr> returnValue);

  std::map<llvm::Value *, ref<Expr> > getLatestCoreExpressions() const;

  std::map<llvm::Value *, std::vector<ref<Expr> > >
  getCompositeCoreExpressions() const;

  std::map<llvm::Value *, ref<Expr> > getLatestInterpolantCoreExpressions(
      std::vector<const Array *> &replacements) const;

  std::map<llvm::Value *, std::vector<ref<Expr> > >
  getCompositeInterpolantCoreExpressions(
      std::vector<const Array *> &replacements) const;

  void computeInterpolantAllocations(AllocationGraph *g);

private:
  ITreeNode(ITreeNode *_parent);

  ~ITreeNode();

  void print(llvm::raw_ostream &stream, const unsigned tabNum) const;
};
}
#endif /* ITREE_H_ */
